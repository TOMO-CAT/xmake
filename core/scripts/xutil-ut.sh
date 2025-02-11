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

# 单测列表
UNIT_TESTS_ARRAY=(
    "algorithm_find"
    "algorithm_sort"
    "container_bloom_filter"
    "memory_buffer"
    "memory_check"
    "utils_trace"
)

function run_all_uts() {
  for ut in "${UNIT_TESTS_ARRAY[@]}"; do
    info "start running ut [${ut}] ..."
    xmake run xutil-ut "${ut}" || exit 1
  done
}

# trace 宏依赖 debug 模式
xmake f -m debug --ut=y
xmake b -vD --all
run_all_uts

# 非 debug 模式
xmake f --ut=y -c
xmake b -vD --all
run_all_uts

ok "run all uts"
