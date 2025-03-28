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

# docker 里无法运行 snap
if [ -f /.dockerenv ]; then
  warning "Running inside a Docker container, quit"
  exit 0
fi

# 安装 zig
# https://github.com/ziglang/zig/wiki/Install-Zig-from-a-Package-Manager
sudo apt install snapd -y || exit 1
sudo snap install zig --classic --beta || exit 1

# 使用 zig 编译 host c / c++ 代码
xmake f --cc="zig cc" --cxx="zig c++" --ld="zig c++" -c
xmake b -vD

# 编译 arm64 架构
xmake f --toolchain=zig -a arm64 -c
xmake b -vD
