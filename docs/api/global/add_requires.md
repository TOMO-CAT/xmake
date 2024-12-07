# add_requires

## 功能

添加需要的依赖包。

## 用法

```lua
-- 禁用系统包 (默认情况下 xmake 会先去检测系统库是否存在)
add_requires("zlib", {system = false})
add_requires("xmake::zlib")

-- 禁用包校验 (一般而言下载的包都会自动校验完整性, 避免被篡改)
add_requires("zlib", {verify = false})

-- 作为私有包 (不对外导出 links / linkdirs)
add_requires("zlib", {private = true})

-- 禁用外部头文件搜索路径 (将 -isystem 切回到 system)
add_requires("zlib 1.x", {external = false})
```
