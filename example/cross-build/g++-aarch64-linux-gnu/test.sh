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
# sudo apt install g++-aarch64-linux-gnu -y || exit 1

# 编译交叉编译版本 (必须用绝对路径, 否则 cmake 会报错找不到 aarch64-none-linux-gnu-g++ 等二进制)
xmake config --yes -p cross --cross=aarch64-linux-gnu- --arch=armv8-a --sdk=/usr
xmake build --verbose --all

output=`file build/cross/armv8-a/release/g++-aarch64-linux-gnu` || exit -1
if [[ "$output" =~ "ARM" && "$output" =~ "aarch64" ]]; then
    ok "output file is ARM aarch64"
else
    error "output file is not ARM aarch64!"
    exit 1
fi

# 编译 host 版本
xmake config --yes --verbose --clean
xmake build --verbose --all --rebuild
output=`file build/linux/x86_64/release/g++-aarch64-linux-gnu` || exit -1
if [[ "$output" =~ "x86-64" ]]; then
    ok "output file is x86_64"
else
    error "output file is not x86_64!"
    exit 1
fi
