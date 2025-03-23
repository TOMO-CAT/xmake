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

exit 0

# 耗时太久暂时不在 ci 开启
if [ -z "${GITHUB_ACTIONS+x}" ]; then
    info "not running in github actions"
else
    info "running in github actions, skip"
    exit 0
fi

xmake build --yes --verbose --rebuild --all
