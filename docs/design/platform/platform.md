# Platform

## 加载 plats

xmake 会从两个目录读取 platform 信息（一般就是 xmake 源码的 `xmake/platforms` 目录）：

```lua
function platform.directories()

    -- init directories
    local dirs = platform._DIRS or  {   path.join(global.directory(), "platforms")
                                    ,   path.join(os.programdir(), "platforms")
                                    }

    -- save directories to cache
    platform._DIRS = dirs
    return dirs
end
```

接着挨个将 xmake 自定义的 `platform` 加载出来。

## 加载 toolchains

入口在 `xmake/core/platform/platform.lua` 里的 `_instance:toolchains` 函数里：

* 先尝试从 config 里获取 toolchain，否则从 platform 里获取 toolchains
* 先从 xmake 里加载 toolchain，找不到则从用户 project 里找到 toolchain 的定义

以 android platform 为例，它定义了三个 tool chain：

```lua
platform("android", function()
    set_os("android")
    set_hosts("macosx", "linux", "windows")
    set_toolchains("envs", "ndk", "rust")
    ...
end)
```

加载 toolchains 时调用的是 `xmake/core/tool/toolchain.lua` 中的 `toolchain.load` 方法，以 envs 为例：

* 拼装出 cachekey：`envs_arch_arm64-v8a_plat_android`
* 根据 cachekey 先从 `toolchain._memcache()` 里读取缓存，读取不到再加载
* 拼接出 toolchain 脚本路径：`@programdir/toolchains/envs/xmake.lua`
* 调用 interp 加载 toolchain 定义
