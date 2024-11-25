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

xmake f --debug=y
xmake -br

is_wsl() {
    if grep -qEi "(Microsoft|WSL)" /proc/version; then
        return 0
    else
        return 1
    fi
}

expected_output_file="expected-build-batchjobs.txt"

diff build/.debug/build-batchjobs.txt "$expected_output_file" > /dev/null
if [ $? -eq 0 ]; then
    info "The jobpool graph string is the same."
else
    error "The jobpool graph string is different."
    exit 1
fi
