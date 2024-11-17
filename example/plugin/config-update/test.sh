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

contains_string() {
    local file="$1"
    local string="$2"
    
    # `--` 用于告诉 grep 选项结束, 后面的是要搜索的字符串
    if grep -q -- "$string" "$file"; then
        return 0
    else
        return 1
    fi
}

assert_contains_string() {
    local file="$1"
    local string="$2"

    if contains_string "$file" "$string"; then
        ok "File $file contains string $string"
    else
        error "File $file does not contain string $string"
        exit 1
    fi
}

xmake f -cv

xmake f --policies=package.precompiled:n,package.install_only
xmake f --export=build/config.txt
assert_contains_string "build/config.txt" "package.precompiled:n"

# xmake f --cxxflags="-DTEST -O3 -Wall"
xmake cu --cxxflags="-DTEST -O3 -Wall"
xmake f --export=build/config.txt
assert_contains_string "build/config.txt" "package.precompiled:n"
assert_contains_string "build/config.txt" "-DTEST -O3 -Wall"

# xmake f --debug=y
xmake cu --debug=y
xmake f --export=build/config.txt
assert_contains_string "build/config.txt" "package.precompiled:n"
assert_contains_string "build/config.txt" "-DTEST -O3 -Wall"
assert_contains_string "build/config.txt" "debug = true"

# xmake f -m coverage
xmake cu -m coverage
xmake f --export=build/config.txt
assert_contains_string "build/config.txt" "package.precompiled:n"
assert_contains_string "build/config.txt" "-DTEST -O3 -Wall"
assert_contains_string "build/config.txt" "coverage"
