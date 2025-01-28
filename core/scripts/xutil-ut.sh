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

# trace 宏依赖 debug 模式
xmake f -m debug --ut=y
xmake b -vD --all

# 单测列表
UNIT_TESTS_ARRAY=(
    "algorithm_sort"
    "memory_buffer"
    "memory_check"
    "utils_trace"
)

# 运行单测
for ut in "${UNIT_TESTS_ARRAY[@]}"; do
    info "start running ut [${ut}] ..."
    xmake run xutil-ut "${ut}" || exit 1
done

ok "run all uts"
