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

rm ./.xmake -rf
rm ./build -rf

# sudo apt install g++-aarch64-linux-gnu -y || exit 1

xmake f --toolchain=cat.aarch64 --arch=armv8-a -y
xmake build -v --all
