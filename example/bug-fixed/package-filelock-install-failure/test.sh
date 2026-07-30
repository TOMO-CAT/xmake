#!/usr/bin/env bash

set -eu

rootdir=$(cd "$(dirname "$0")" && pwd)
# 两个项目共用全局包目录以竞争同一个包文件锁
globaldir="$rootdir/.xmake-global"
markerdir="$rootdir/.markers"
log1="$rootdir/first.log"
log2="$rootdir/second.log"
log3="$rootdir/after-holder-exit.log"
pid1=""
xmake_bin=$(command -v xmake)

echo "testing xmake: $xmake_bin"

stop_first_process() {
    if [ -n "$pid1" ]; then
        kill "$pid1" 2>/dev/null || true
        wait "$pid1" 2>/dev/null || true
        pid1=""
    fi
}
cleanup() {
    # 无论测试成功、失败还是被中断，都不遗留后台 xmake 进程
    stop_first_process
}
trap cleanup EXIT INT TERM

# 清理上次运行产生的缓存、构建目录、同步标记和日志
rm -rf "$globaldir" "$markerdir" \
       "$rootdir/project-a/.xmake" "$rootdir/project-a/build" \
       "$rootdir/project-b/.xmake" "$rootdir/project-b/build" \
       "$log1" "$log2" "$log3"
mkdir -p "$markerdir"

# project-a 同时触发一个故意失败的安装和一个慢安装
(
    cd "$rootdir/project-a"
    exec env \
        FILELOCK_MARKER_DIR="$markerdir" XMAKE_GLOBALDIR="$globaldir" \
        "$xmake_bin" f -c -y
) >"$log1" 2>&1 &
pid1=$!

# 等待两个安装任务都已启动
for _ in $(seq 1 100); do
    if [ -f "$markerdir/failed-started" ] && [ -f "$markerdir/slow-started" ]; then
        break
    fi
    if ! kill -0 "$pid1" 2>/dev/null; then
        echo "first xmake process exited before the hang was established"
        cat "$log1"
        exit 1
    fi
    sleep 0.1
done
if [ ! -f "$markerdir/failed-started" ] || [ ! -f "$markerdir/slow-started" ]; then
    echo "failed: package install jobs did not start in time"
    cat "$log1"
    exit 1
fi

# 确认失败已经发生, 此时慢安装仍用于延长第一个进程的生命周期
for _ in $(seq 1 100); do
    if grep -q "intentional package_with_failed_install failure" "$log1" 2>/dev/null; then
        break
    fi
    if ! kill -0 "$pid1" 2>/dev/null; then
        echo "first xmake process exited before reporting the expected install failure"
        cat "$log1"
        exit 1
    fi
    sleep 0.1
done
if ! grep -q "intentional package_with_failed_install failure" "$log1" 2>/dev/null; then
    echo "failed: the expected package install failure was not reported in time"
    cat "$log1"
    exit 1
fi

# 在第一个进程尚未完全退出时运行 project-b, 验证失败任务已主动释放包锁
set +e
(
    cd "$rootdir/project-b"
    timeout 8s env \
        FILELOCK_MARKER_DIR="$markerdir" XMAKE_GLOBALDIR="$globaldir" \
        "$xmake_bin" f -c -y >"$log2" 2>&1
)
status2=$?
set -e

# 124 超时
if [ "$status2" -eq 124 ]; then
    echo "failed: the second project blocked on the package lock"
    cat "$log1"
    cat "$log2"

    stop_first_process

    # 杀掉持锁进程后重试, 用于确认锁是否只能依靠进程退出释放
    set +e
    (
        cd "$rootdir/project-b"
        timeout 4s env \
            FILELOCK_MARKER_DIR="$markerdir" XMAKE_GLOBALDIR="$globaldir" \
            "$xmake_bin" f -c -y >"$log3" 2>&1
    )
    set -e
    if ! grep -q "intentional package_with_failed_install failure" "$log3"; then
        echo "failed: the package lock remained active after its holder exited"
        cat "$log3"
    else
        echo "confirmed: the package lock was released after its holder exited"
    fi
    exit 1
fi

# project-b 完成后, 等待 project-a 的慢安装收尾并退出
for _ in $(seq 1 100); do
    if ! kill -0 "$pid1" 2>/dev/null; then
        break
    fi
    sleep 0.1
done
if kill -0 "$pid1" 2>/dev/null; then
    echo "failed: the first xmake process did not exit after the install failure"
    cat "$log1"
    exit 1
fi

set +e
wait "$pid1"
status1=$?
set -e
pid1=""

# 两个项目都应到达预期的安装失败, 因此退出状态必须为非零
if [ "$status1" -eq 0 ] || [ "$status2" -eq 0 ]; then
    echo "failed: both projects should report the intentional package install failure"
    cat "$log1"
    cat "$log2"
    exit 1
fi
if ! grep -q "intentional package_with_failed_install failure" "$log2"; then
    echo "failed: the second project did not reach the expected install failure"
    cat "$log2"
    exit 1
fi

echo "passed: install failures released the package lock without blocking another project"

bash "$rootdir/test-fetch.sh"
