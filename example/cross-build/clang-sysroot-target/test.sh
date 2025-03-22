#!/bin/bash

set -e

function info() {
  (>&2 printf "[\e[34m\e[1mINFO\e[0m] $*\n")
}

function error() {
  (>&2 printf "[\033[0;31mERROR\e[0m] $*\n")
}

function warning() {
  (>&2 printf "[\033[0;33mWARNING\e[0m] $*\n")
}

function ok() {
  (>&2 printf "[\e[32m\e[1m OK \e[0m] $*\n")
}

# 安装交叉编译工具链
sudo apt install g++-aarch64-linux-gnu -y || exit 1

# 交叉编译必须禁用掉对应的环境变量(这相当于环境变量是最高优先级?), 否则会用 host clang++ 以及错误的系统头文件
unset CPLUS_INCLUDE_PATH
unset C_INCLUDE_PATH
unset CPP
unset CC
unset CXX
unset LD

sudo apt install -y protobuf-compiler

# --toolchain=aarch64-clang: 指定 toolchain, 只能在最外层 includes
# --verbose: 详细信息
# --diagnosis: 调试信息
# --arch=armv8-a: 指定 arch, 这样产出物就会在 build/linux/armv8-a/release 目录下, 不会放在 build/linux/x86_64 下迷惑人
xmake f --yes --toolchain=aarch64-clang --arch=armv8-a
xmake b --yes --verbose --diagnosis --rebuild --all

output=`file build/linux/armv8-a/release/clang-sysroot-target` || exit -1
if [[ "$output" =~ "ARM" && "$output" =~ "aarch64" ]]; then
    ok "output file is ARM aarch64"
else
    error "output file is not ARM aarch64!"
    exit 1
fi

# 编译 host 版本
xmake config --yes --verbose --clean
xmake build --verbose --all
output=`file build/linux/x86_64/release/clang-sysroot-target` || exit -1
if [[ "$output" =~ "x86-64" ]]; then
    ok "output file is x86_64"
else
    error "output file is not x86_64!"
    exit 1
fi
