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
sudo apt install g++-9-aarch64-linux-gnu || exit 1
sudo mkdir -p /usr/aarch64-linux-gnu/usr/aarch64-linux-gnu
sudo ln -svnf /usr/aarch64-linux-gnu/lib /usr/aarch64-linux-gnu/usr/aarch64-linux-gnu/lib

# 交叉编译必须禁用掉对应的环境变量(这相当于环境变量是最高优先级?), 否则会用 host clang++ 以及错误的系统头文件
unset CPLUS_INCLUDE_PATH
unset C_INCLUDE_PATH
unset CPP
unset CC
unset CXX
unset LD

# 编译交叉编译版本 (必须用绝对路径, 否则 cmake 会报错找不到 aarch64-none-linux-gnu-g++ 等二进制)
xmake f -yD -p cross --cross=aarch64-linux-gnu- --arch=armv8-a --toolchain=orin --sdk=/usr
xmake b -yvrD

output=`file build/cross/armv8-a/release/g++-9-aarch64-linux-gnu-cross` || exit -1
if [[ "$output" =~ "ARM" && "$output" =~ "aarch64" ]]; then
    ok "output file is ARM aarch64"
else
    error "output file is not ARM aarch64!"
    exit 1
fi

# 编译 host 版本
xmake f -cyD
xmake b -yvrD
output=`file build/linux/x86_64/release/g++-9-aarch64-linux-gnu-cross` || exit -1
if [[ "$output" =~ "x86-64" ]]; then
    ok "output file is x86_64"
else
    error "output file is not x86_64!"
    exit 1
fi
