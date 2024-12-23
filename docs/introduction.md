# Introduction

## 支持工具链

```bash
$ xmake show -l toolchains
xcode         Xcode IDE
yasm          The Yasm Modular Assembler
clang         A C language family frontend for LLVM
go            Go Programming Language Compiler
dlang         D Programming Language Compiler (Auto)
dmd           D Programming Language Compiler
ldc           The LLVM-based D Compiler
gdc           The GNU D Compiler (GDC)
gfortran      GNU Fortran Programming Language Compiler
zig           Zig Programming Language Compiler
sdcc          Small Device C Compiler
cuda          CUDA Toolkit (nvcc, nvc, nvc++, nvfortran)
ndk           Android NDK
rust          Rust Programming Language Compiler
swift         Swift Programming Language Compiler
llvm          A collection of modular and reusable compiler and toolchain technologies
cross         Common cross compilation toolchain
nasm          NASM Assembler
gcc           GNU Compiler Collection
gnu-rm        GNU Arm Embedded Toolchain
envs          Environment variables toolchain
fasm          Flat Assembler
tinycc        Tiny C Compiler
emcc          A toolchain for compiling to asm.js and WebAssembly
icc           Intel C/C++ Compiler
ifort         Intel Fortran Compiler
ifx           Intel LLVM Fortran Compiler
muslcc        The musl-based cross-compilation toolchain
fpc           Free Pascal Programming Language Compiler
wasi          WASI-enabled WebAssembly C/C++ toolchain
nim           Nim Programming Language Compiler
circle        A new C++20 compiler
c51           Keil development tools for the 8051 Microcontroller Architecture
icx           Intel LLVM C/C++ Compiler
dpcpp         Intel LLVM C++ Compiler for data parallel programming model based on Khronos SYCL
iverilog      Icarus Verilog
verilator     Verilator open-source SystemVerilog simulator and lint system
cosmocc       build-once run-anywhere
hdk           Harmony SDK
```

## 支持平台

* Windows (x86, x64, arm, arm64, arm64ec)
* macOS (i386, x86_64, arm64)
* Linux (i386, x86_64, arm, arm64, riscv, mips, 390x, sh4 ...)
* *BSD (i386, x86_64)
* Android (x86, x86_64, armeabi, armeabi-v7a, arm64-v8a)
* iOS (armv7, armv7s, arm64, i386, x86_64)
* WatchOS (armv7k, i386)
* AppleTVOS (armv7, arm64, i386, x86_64)
* AppleXROS (arm64, x86_64)
* Wasm (wasm32, wasm64)
* Haiku (i386, x86_64)
* Harmony (x86_64, armeabi-v7a, arm64-v8a)
* Cross (cross-toolchains ..)

## 工程类型

* 静态库程序
* 动态库类型
* 控制台程序
* Cuda 程序
* Qt 应用程序
* WDK Windows 驱动程序
* WinSDK 应用程序
* MFC 应用程序
* iOS/MacOS 应用程序（支持.metal）
* Framework和Bundle程序（iOS/MacOS）
* SWIG/Pybind11 模块 (Lua, python, ...)
* Luarocks 模块
* Protobuf 程序
* Lex/yacc 程序
* C++20 模块
* Linux 内核驱动模块
* Keil MDK/C51 嵌入式程序
* Verilog 仿真程序

## 分布式编译和缓存

* [x] 跨平台支持
* [x] 支持 clang, gcc 和交叉编译工具链
* [x] 支持构建 android, ios, linux, win, macOS 程序
* [x] 除了编译工具链，无任何其他依赖
* [x] 支持编译服务器负载均衡调度
* [x] 支持大文件实时压缩传输 (lz4)
* [x] 几乎零配置成本，无需共享文件系统，更加方便和安全

关于分布式编译和缓存，可以见下面的文档。

* [分布式编译](https://xmake.io/#/zh-cn/features/distcc_build)
* [编译缓存](https://xmake.io/#/zh-cn/features/build_cache)

## 远程编译

更多详情见：[远程编译](https://xmake.io/#/zh-cn/features/remote_build)

## IDE和编辑器插件

* [xmake-vscode](https://github.com/xmake-io/xmake-vscode)

<img src="https://raw.githubusercontent.com/xmake-io/xmake-vscode/master/res/problem.gif" width="650px" />

* [xmake-sublime](https://github.com/xmake-io/xmake-sublime)

<img src="https://raw.githubusercontent.com/xmake-io/xmake-sublime/master/res/problem.gif" width="650px" />

* [xmake-idea](https://github.com/xmake-io/xmake-idea)

<img src="https://raw.githubusercontent.com/xmake-io/xmake-idea/master/res/problem.gif" width="650px" />

* [xmake.vim](https://github.com/luzhlon/xmake.vim) (third-party, thanks [@luzhlon](https://github.com/luzhlon))

* [xmake-visualstudio](https://github.com/HelloWorld886/xmake-visualstudio) (third-party, thanks [@HelloWorld886](https://github.com/HelloWorld886))

* [xmake-qtcreator](https://github.com/Arthapz/xmake-project-manager) (third-party, thanks [@Arthapz](https://github.com/Arthapz))

## CI 集成

### 1. GitHub Action

我们可以使用 [github-action-setup-xmake](https://github.com/xmake-io/github-action-setup-xmake) 在 Github Action 上实现跨平台安装集成 Xmake。

```bash
uses: xmake-io/github-action-setup-xmake@v1
with:
  xmake-version: latest
```
