# toolchain

## 加载

入口在 `xmake/core/tool/toolchain.lua` 中：

```lua
function _instance:_load()
    ...
end
```

* 读取 `info.__loaded` 和 `info.__loading`，避免给每个 arch 重复加载
* 获取 `self.on_load` 方法，如果读取不到则用  `self.load_cross_toolchain` 作为 `self.on_load` 方法
  * 对于系统内置的 toolchain （以 ndk 为例）而言，他的 `on_load` 方法定义在 `xmake/toolchains/ndk/load.lua` 中

## checktool

> 例子在 `example/cross-build/android` 中。

入口在 `xmake/core/tool/toolchain.lua` 里的 `_instance:_checktool` 函数（以 toolkind = sh ，toolpath = clang++为例）：

* 先构造 cachekey："ndk_arch_arm64-v8a_plat_android_checktoolsh"
* 根据 cachekey 先从 `toolchain._memcache()` 里读取缓存，读取不到再 check
* 调用 `import("lib.detect.find_tool")` 找 toolpath，由于我们指定了 sdk，因此会优先从 sdk 里读取 `checking for clang++ ... android-ndk-r22/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++`
