# Usage

## 安装

```bash
bash scripts/install.sh
```

## 简单的工程描述

```lua
target("hello")
    set_kind("binary")
    add_files("src/*.cpp")
```

## 包依赖描述

```lua
add_requires("tbox 1.6.*", "zlib", "libpng ~1.6")
```

官方的xmake包管理仓库: [xmake-repo](https://github.com/xmake-io/xmake-repo)

<img src="https://github.com/xmake-io/xmake-docs/raw/master/assets/img/index/package.gif" width="650px" />

## 命令行使用

### 1. 创建工程

```bash
xmake create hello
cd hello
```

### 2. 构建工程

```bash
xmake
```

### 3. 运行目标

```bash
xmake run console
```

### 4. 调试程序

```bash
xmake run -d console
```

### 5. 运行测试

```bash
xmake test
```

### 6. 配置平台

```bash
xmake f -p [linux|macosx|android|iphoneos ..] -a [x86|arm64 ..] -m [debug|release]
xmake
```

### 7. 图形化菜单配置

```bash
xmake f --menu
```

<img src="https://xmake.io/assets/img/index/menuconf.png" width="650px" />

## 更多例子

### 1. Debug 和 Release 模式

```lua
add_rules("mode.debug", "mode.release")

target("console")
    set_kind("binary")
    add_files("src/*.c")
    if is_mode("debug") then
        add_defines("DEBUG")
    end
```

### 2. 自定义脚本

```lua
target("test")
    set_kind("binary")
    add_files("src/*.c")
    after_build(function (target)
        print("hello: %s", target:name())
        os.exec("echo %s", target:targetfile())
    end)
```

### 3. 依赖包自动集成

下载和使用在 [xmake-repo](https://github.com/xmake-io/xmake-repo) 和第三方包仓库的依赖包：

```lua
add_requires("tbox >1.6.1", "libuv master", "vcpkg::ffmpeg", "brew::pcre2/libpcre2-8")
add_requires("conan::openssl/1.1.1g", {alias = "openssl", optional = true, debug = true})
target("test")
    set_kind("binary")
    add_files("src/*.c")
    add_packages("tbox", "libuv", "vcpkg::ffmpeg", "brew::pcre2/libpcre2-8", "openssl")
```

另外，我们也可以使用 [xrepo](https://github.com/xmake-io/xrepo) 命令来快速安装依赖包。

### 4. Qt QuickApp 应用程序

```lua
target("test")
    add_rules("qt.quickapp")
    add_files("src/*.cpp")
    add_files("src/qml.qrc")
```

### 5. Cuda 程序

```lua
target("test")
    set_kind("binary")
    add_files("src/*.cu")
    add_cugencodes("native")
    add_cugencodes("compute_35")
```

### 6. IOS/MacOS 应用程序

```lua
target("test")
    add_rules("xcode.application")
    add_files("src/*.m", "src/**.storyboard", "src/*.xcassets")
    add_files("src/Info.plist")
```

### 7. Framework 和 Bundle 程序（iOS/MacOS）

```lua
target("test")
    add_rules("xcode.framework") -- 或者 xcode.bundle
    add_files("src/*.m")
    add_files("src/Info.plist")
```

### 8. OpenMP 程序

```lua
add_requires("libomp", {optional = true})
target("loop")
    set_kind("binary")
    add_files("src/*.cpp")
    add_rules("c++.openmp")
    add_packages("libomp")
```

### 9. Zig 程序

```lua
target("test")
    set_kind("binary")
    add_files("src/main.zig")
```

## 自动拉取远程工具链

### 1. 拉取指定版本的 llvm 工具链

我们使用 llvm-10 中的 clang 来编译项目。

```lua
add_requires("llvm 10.x", {alias = "llvm-10"})
target("test")
    set_kind("binary")
    add_files("src/*.c")
    set_toolchains("llvm@llvm-10")
```

### 2. 拉取交叉编译工具链

我们也可以拉取指定的交叉编译工具链来编译项目。

```lua
add_requires("muslcc")
target("test")
    set_kind("binary")
    add_files("src/*.c")
    set_toolchains("@muslcc")
```

### 3. 拉取工具链并且集成对应工具链编译的依赖包

我们也可以使用指定的muslcc交叉编译工具链去编译和集成所有的依赖包。

```lua
add_requires("muslcc")
add_requires("zlib", "libogg", {system = false})

set_toolchains("@muslcc")

target("test")
    set_kind("binary")
    add_files("src/*.c")
    add_packages("zlib", "libogg")
```

## 插件

### 1. 生成IDE工程文件插件（makefile, vs2002 - vs2022, ...）

```bash
xmake project -k vs -m "debug,release"
xmake project -k cmake
xmake project -k ninja
xmake project -k compile_commands
```

### 2. 加载自定义lua脚本插件

```bash
$ xmake l ./test.lua
$ xmake l -c "print('hello xmake!')"
$ xmake l lib.detect.find_tool gcc
$ xmake l
> print("hello xmake!")
> {1, 2, 3}
< {
    1,
    2,
    3
  }
```

更多内置插件见相关文档：[内置插件文档](https://xmake.io/#/zh-cn/plugin/builtin_plugins)

其他扩展插件，请到插件仓库进行下载安装: [xmake-plugins](https://github.com/xmake-io/xmake-plugins).
