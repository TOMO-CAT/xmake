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

# 通过 snap 安装 zig
# https://github.com/ziglang/zig/wiki/Install-Zig-from-a-Package-Manager
# sudo apt install snapd -y || exit 1
# sudo snap install zig --classic --beta || exit 1
if [ -n "${GITHUB_ACTIONS+x}" ]; then
    info "running in github actions, skipping"
    exit 0
fi

# 通过官方 tar 包安装 zig（已存在则跳过）
if command -v zig >/dev/null 2>&1; then
  info "zig already installed: $(command -v zig) ($(zig version))"
else
  ZIG_VER=0.13.0
  curl -LO https://ziglang.org/download/${ZIG_VER}/zig-linux-x86_64-${ZIG_VER}.tar.xz
  sudo tar -C /opt -xf zig-linux-x86_64-${ZIG_VER}.tar.xz
  sudo ln -sf /opt/zig-linux-x86_64-${ZIG_VER}/zig /usr/local/bin/zig
  zig version
fi

# 使用 zig 编译 host c / c++ 代码
xmake f --cc="zig cc" --cxx="zig c++" --ld="zig c++" -c
xmake b

# 编译 arm64 架构
xmake f --toolchain=zig -a arm64 --clean
xmake b
