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

xmake_build_output=`xmake -br` || exit -1
matching_lines=$(echo "$xmake_build_output" | grep "cache compiling.release")

if [[ -z "$matching_lines" ]]; then
    error "No lines found containing 'cache compiling.release'."
    echo "${xmake_build_output}"
    exit 1
fi

first_matching_line=$(echo "$matching_lines" | head -n 1)

if [[ "$first_matching_line" != *"cache compiling.release src/high_priority.cc"* ]]; then
    error "The first matching line does not contain 'cache compiling.release src/high_priority.cc'."
    exit 1
fi

ok "high priority.cc was compiled first"
