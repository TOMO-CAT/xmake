#!/usr/bin/env bash

set -eu

rootdir=$(cd "$(dirname "$0")" && pwd)
# 两个项目共用全局包目录以竞争同一个包文件锁
globaldir="$rootdir/.xmake-global"
markerdir="$rootdir/.markers"
log1="$rootdir/fetch-first.log"
log2="$rootdir/fetch-second.log"
log3="$rootdir/fetch-after-holder-exit.log"
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
       "$rootdir/project-fetch-a/.xmake" "$rootdir/project-fetch-a/build" \
       "$rootdir/project-fetch-b/.xmake" "$rootdir/project-fetch-b/build" \
       "$log1" "$log2" "$log3"
mkdir -p "$markerdir"

# project-fetch-a 同时触发一个故意失败的 fetch 和一个慢 fetch
(
    cd "$rootdir/project-fetch-a"
    exec env \
        FILELOCK_MARKER_DIR="$markerdir" XMAKE_GLOBALDIR="$globaldir" \
        "$xmake_bin" f -c -y
) >"$log1" 2>&1 &
pid1=$!

# 等待两个 fetch 任务都已启动
for _ in $(seq 1 100); do
    if [ -f "$markerdir/failed-fetch-started" ] && [ -f "$markerdir/slow-fetch-started" ]; then
        break
    fi
    if ! kill -0 "$pid1" 2>/dev/null; then
        echo "first xmake process exited before the hang was established"
        cat "$log1"
        exit 1
    fi
    sleep 0.1
done
if [ ! -f "$markerdir/failed-fetch-started" ] || [ ! -f "$markerdir/slow-fetch-started" ]; then
    echo "failed: package fetch jobs did not start in time"
    cat "$log1"
    exit 1
fi

# 等待失败 fetch 即将抛错。错误日志要等其他 job 结束后才会输出，不能用日志同步。
for _ in $(seq 1 100); do
    if [ -f "$markerdir/failed-fetch-ready" ]; then
        break
    fi
    if ! kill -0 "$pid1" 2>/dev/null; then
        echo "first xmake process exited before reporting the expected fetch failure"
        cat "$log1"
        exit 1
    fi
    sleep 0.1
done
if [ ! -f "$markerdir/failed-fetch-ready" ]; then
    echo "failed: the expected package fetch failure was not reached in time"
    cat "$log1"
    exit 1
fi

# 在第一个进程尚未完全退出时运行 project-fetch-b, 验证失败任务已主动释放包锁
set +e
(
    cd "$rootdir/project-fetch-b"
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
        cd "$rootdir/project-fetch-b"
        timeout 4s env \
            FILELOCK_MARKER_DIR="$markerdir" XMAKE_GLOBALDIR="$globaldir" \
            "$xmake_bin" f -c -y >"$log3" 2>&1
    )
    set -e
    if grep -q "filelock .* is being accessed by other process" "$log3"; then
        echo "failed: the package lock remained active after its holder exited"
        cat "$log3"
    else
        echo "confirmed: the package lock was released after its holder exited"
    fi
    exit 1
fi

# project-fetch-b 完成后, 等待 project-fetch-a 的慢 fetch 收尾并退出
for _ in $(seq 1 100); do
    if ! kill -0 "$pid1" 2>/dev/null; then
        break
    fi
    sleep 0.1
done
if kill -0 "$pid1" 2>/dev/null; then
    echo "failed: the first xmake process did not exit after the fetch failure"
    cat "$log1"
    exit 1
fi

set +e
wait "$pid1"
status1=$?
set -e
pid1=""

# 两个项目都应到达预期的 fetch 失败, 因此退出状态必须为非零
if [ "$status1" -eq 0 ] || [ "$status2" -eq 0 ]; then
    echo "failed: both projects should report the intentional package fetch failure"
    cat "$log1"
    cat "$log2"
    exit 1
fi
if ! grep -q "intentional package_with_failed_fetch failure" "$log2"; then
    echo "failed: the second project did not reach the expected fetch failure"
    cat "$log2"
    exit 1
fi

echo "passed: fetch failures released the package lock without blocking another project"
