add_rules("mode.debug", "mode.release")

-- 等价于在命令行中指定 xmake f 参数 (对于需要同时编译 cross 版本和 host 版本的通过命令行指定比较方便, 我们也可以定义 custom toolchain 将这些参数收敛起来):
-- xmake f -p android --ndk=android-ndk-r22 --ndk_sdkver=21 -a armeabi-v7a --cc=android-ndk-r22/toolchains/llvm/prebuilt/linux-x86_64/bin/clang --cxx=android-ndk-r22/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ --ld=android-ndk-r22/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++

-- 如果当前项目只是为了交叉编译, 那么可以直接 set_config 指定 xmake config
set_config("plat", "android")
set_config("ndk", "android-ndk-r22")
set_config("ndk_sdkver", 21)
set_config("arch", "arm64-v8a")

-- 给所有的 target 设置内置的 toolchain, 否则会找到 host clang++ 编译
-- @see https://github.com/TOMO-CAT/xmake/issues/134
set_toolchains("ndk")

target("android-cross", function()
    set_kind("binary")
    add_files("src/*.cc")
end)
