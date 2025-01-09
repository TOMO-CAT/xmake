-- 设置允许编译的平台列表, 如果用户指定了其他平台, 会提示错误
set_allowedplats("linux")

-- 设置允许编译的平台架构
set_allowedarchs("x64", "x86")
-- 也可以同时指定多个平台下允许的架构列表 (linux 上仅支持 x64; iphoneos 上仅支持 arm64)
set_allowedarchs("linux|x64", "iphoneos|arm64")

-- 设置允许的编译模式列表
set_allowedmodes("release", "releasedbg")
