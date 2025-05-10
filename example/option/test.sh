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

# 默认情况下 dev 不启用
output=`xmake f -cvD`
if [[ "$output" =~ "=== bar ===" && "$output" =~ "dev = false" ]]; then
    ok "dev is not enabled"
else
    error "dev is enabled!"
    exit 1
fi

# 启用 dev
output=`xmake f --dev=y -vD`
if [[ "$output" =~ "=== foo ===" && "$output" =~ "dev = true" ]]; then
    ok "dev is enabled"
else
    error "dev is not enabled!"
    exit 1
fi

# 触发交叉编译
xmake config -p cross -a arm64-v8a --sdk=/usr --cross=aarch64-linux-gnu-
xmake build --verbose --all --rebuild
output=`file build/cross/arm64-v8a/release/test` || exit -1
if [[ "$output" =~ "ARM aarch64" ]]; then
    ok "output file is aarch64"
else
    error "output file is not ARM aarch64!"
    exit 1
fi
