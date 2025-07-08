#!/bin/bash

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

if [ -z "${GITHUB_ACTIONS+x}" ]; then
    info "not running in github actions"
    xmake build --yes --all || exit 1
else
    # FIXME: 在 github action 上 xgpu 会一直不链接 protobuf-cpp, 暂时关闭
    warning "running in github actions, skip it because it may fail"
fi
