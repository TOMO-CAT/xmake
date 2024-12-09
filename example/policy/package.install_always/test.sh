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

xmake_build_output=`xmake b -r -y` || exit -1
matching_lines=$(echo "$xmake_build_output" | grep -F "skip installed package [xproto")

if [[ -z "$matching_lines" ]]; then
    error "No lines found containing 'skip installed package [xproto'."
    echo "${xmake_build_output}"
    exit 1
fi

ok "skip repeatedly install package with package.install_always policy"
