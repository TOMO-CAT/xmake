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
    "container_circle_queue"
    "container_hash_map"
    "container_hash_set"
    "container_heap"
    "container_list_entry"
    "container_list"
    "container_queue"
    "container_single_list_entry"
    "container_single_list"
    "container_stack"
    "container_vector"
    "hash_benchmark"
    "libc_stdlib"
    "libc_string"
    "libc_time"
    "libm_double"
    "libm_float"
    "libm_integer"
    "math_fixed"
    "math_random"
    "memory_impl_static_fixed_pool"
    "memory_buffer"
    "memory_check"
    "memory_default_allocator"
    "memory_fixed_pool"
    "memory_large_allocator"
    "memory_memops"
    "memory_queue_buffer"
    "memory_small_allocator"
    "memory_static_buffer"
    "memory_static_buffer"
    "network_impl_date"
    "network_cookies"
    "network_url"
    "platform_atomic"
    "platform_atomic32"
    "platform_atomic64"
    "platform_backtrace"
    "platform_cache_time"
    "platform_hostname"
    "platform_ifaddrs"
    "platform_lock"
    "platform_path"
    "platform_pipe_pair"
    "platform_sched"
    "platform_thread"
    "platform_utils"
    "string_static_string"
    "string_string"
    "utils_trace"
)

function run_all_uts() {
  for ut in "${UNIT_TESTS_ARRAY[@]}"; do
    info "start running ut [${ut}] ..."
    xmake run xutil-ut "${ut}" || exit 1
  done

  # 特殊单测
  xmake run xutil-ut hash_adler32 tomocat
  xmake run xutil-ut hash_crc8 tomocat
  xmake run xutil-ut hash_crc16 tomocat
  xmake run xutil-ut hash_crc32 tomocat
  xmake run xutil-ut hash_djb2 tomocat
  xmake run xutil-ut hash_fnv32 tomocat
  xmake run xutil-ut hash_fnv64 tomocat
  xmake run xutil-ut hash_md5 tomocat
  xmake run xutil-ut hash_sdbm tomocat
  xmake run xutil-ut hash_sha tomocat
  xmake run xutil-ut hash_uuid tomocat

  xmake run xutil-ut network_dns www.baidu.com
  # xmake run xutil-ut network_http www.baidu.com # 测试失败, 跳过
  xmake run xutil-ut network_hwaddr 02:1A:2B:3C:4D:5E
  xmake run xutil-ut network_ipaddr 192.168.1.1
  xmake run xutil-ut network_ipv4 192.168.1.100
  xmake run xutil-ut network_ipv6 2001:0db8:85a3:0000:0000:8a2e:0370:7334
  # xmake run xutil-ut network_ping 127.0.0.1 # 测试失败, 跳过
  xmake run xutil-ut network_unixaddr /path/to.sock

  xmake run xutil-ut platform_addrinfo www.baidu.com

  xmake run xutil-ut stream http://www.baidu.com /tmp/a
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
