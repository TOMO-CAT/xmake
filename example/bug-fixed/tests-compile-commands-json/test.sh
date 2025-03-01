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

rm -f ./compile_commands.json
rm -rf ./build
rm -rf ./.xmake

# xmake 不能为 test 生成 compile commands:
# @see https://github.com/xmake-io/xmake/issues/4750
xmake project -k compile_commands --lsp=clangd

if ! grep b.cc compile_commands.json; then
    error "cannot generate compile-commands for b.cc"
    exit 1
fi

ok "generate compile-commands for b.cc"
