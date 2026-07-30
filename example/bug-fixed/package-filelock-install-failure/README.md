# Package fetch/install 失败后进程持续持有 filelock

## 问题背景

xmake 使用全局 package 目录保存依赖的源码、安装结果和 filelock。多个项目共享同一个
`XMAKE_GLOBALDIR` 是正常且必要的：filelock 可以避免多个 xmake 进程同时修改同一个 package，
从而保证全局缓存的一致性。

问题发生在 package fetch 或安装任务已经获取锁，但 `on_fetch`、`on_install` 等逻辑抛出异常时。原实现会
跳过解锁和调度状态清理，使当前 xmake 进程无法退出，并持续持有操作系统文件锁。其他项目
随后安装同一个 package 时，就会一直等待该锁。

这不是进程退出后残留的无效锁。只要真正持锁的进程退出，操作系统就会自动释放文件锁；
问题在于异常路径使持锁进程一直存活。

## 测试场景

install 测试启动两个共享同一 `XMAKE_GLOBALDIR` 的项目，并使用三个本地 package：

- `package_with_failed_install`：在 `on_install` 中主动抛出错误。
- `package_with_failed_dep`：依赖 `package_with_failed_install`，用于等待失败依赖。
- `package_with_slow_install`：安装时等待三秒，保证失败发生时仍有并行任务运行。

依赖和并发关系如下：

```text
project-a
├── package_with_failed_dep
│   └── package_with_failed_install  ← 故意安装失败
└── package_with_slow_install        ← 并行安装三秒

project-b
└── package_with_failed_dep
    └── package_with_failed_install  ← 访问同一个全局 package 锁
```

fetch 测试使用相同的并发结构：

- `package_with_failed_fetch`：在 `on_fetch` 中主动抛出错误。
- `package_with_failed_fetch_dep`：依赖 `package_with_failed_fetch`，用于等待失败依赖。
- `package_with_slow_fetch`：fetch 时等待三秒，使失败发生后第一个进程仍然存活。
- `project-fetch-a` 和 `project-fetch-b`：竞争 `package_with_failed_fetch` 的同一个文件锁。

未修复时的执行过程：

1. `package_with_failed_install` 获取全局 package 锁。
2. `package_with_failed_install` 和 `package_with_slow_install` 并行安装。
3. `package_with_failed_install` 抛出异常，跳过 `instance:unlock()`。
4. `working_count`、`installing_count` 等调度状态也未恢复。
5. `package_with_failed_dep` 持续等待，导致第一个 xmake 进程无法退出。
6. 第二个项目阻塞在 `package_with_failed_install` 的 filelock 上。

典型等待信息如下：

```text
package(package_with_failed_install 1.0.0) filelock [...] is being accessed by other process
```

## 根因与修复

原逻辑只在成功路径释放锁：

```lua
instance:lock()
action_download(instance)
action_install(instance)
instance:unlock()
```

fetch、下载、安装或 package 回调抛出异常后，后续解锁和调度状态更新都不会执行。仅补充解锁仍然
不够，因为错误的调度计数也会使当前进程永久等待。

修复同时覆盖 fetch 和 install 路径：

1. 使用 `package_locked` 记录是否已经成功获取 package 锁。
2. 使用 `finally` 保证正常和异常路径都会执行清理。
3. 在 `finally` 中恢复工作计数、并行状态、package group 状态和任务记录。
4. 如果 `package_locked` 仍为 `true`，立即调用 `instance:unlock()`。

正常路径会主动解锁并将 `package_locked` 设为 `false`，因此 `finally` 不会重复解锁；异常
路径则由 `finally` 完成清理和解锁。

## 修复后的行为

fetch 或安装失败后，失败任务立即进入 `finally`：

1. 恢复调度状态并释放失败 package 的全局锁。
2. 已经开始的 `package_with_slow_install` 可以继续完成。
3. `package_with_failed_dep` 检测到依赖失败并报告错误。
4. 第一个 xmake 进程以失败状态正常退出。
5. 其他项目可以获取同一个 package 锁，不再被前一个失败进程永久阻塞。

全局 package 缓存和 filelock 机制保持不变，多个项目并发访问同一 package 时仍然受到保护。

## 运行回归测试

```sh
./test.sh
```

脚本无交互，可直接在流水线中运行。`test.sh` 会先测试 install 失败，再调用 `test-fetch.sh`
测试 fetch 失败。两个脚本都使用 `PATH` 中的 xmake 可执行文件，并继承调用环境中的
`XMAKE_PROGRAM_DIR`。

测试当前 checkout 中的 Lua 程序文件时，可以运行：

```sh
XMAKE_PROGRAM_DIR=/path/to/xmake/xmake ./test.sh
```

测试结果：

- **未修复版本**：第二个项目等待 filelock 超时，脚本终止持锁进程，并确认进程退出后锁可
  被重新获取；由于检测到跨项目阻塞，脚本返回失败。
- **修复版本**：两个项目都到达预期的 package fetch 或安装失败，但不会互相永久阻塞，后台进程也
  能自行退出，脚本返回成功。
