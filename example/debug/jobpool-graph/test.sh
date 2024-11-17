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


xmake -br

diff build/.debug/build-batchjobs.txt expected-build-batchjobs.txt > /dev/null
if [ $? -eq 0 ]; then
    info "The jobpool graph string is the same."
else
    error "The jobpool graph string is different."
    exit 1
fi
