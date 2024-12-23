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

info "check ndk"
NDK_ZIP_FILE="android-ndk-r22-linux-x86_64.zip"
NDK_DIR="android-ndk-r22"

if [ -d "${NDK_DIR}" ]; then
    warning "dir [${NDK_DIR}] exists, skip installing"
else
    if [ -f "${NDK_ZIP_FILE}" ]; then
        warning "file [${NDK_ZIP_FILE}] exists. skip downloading"
    else
        info "start downloading ndk zip file [${NDK_ZIP_FILE}]"
        wget "https://dl.google.com/android/repository/android-ndk-r22-linux-x86_64.zip" -O android-ndk-r22-linux-x86_64.zip
    fi
    set -x
    unzip -q -o ./android-ndk-r22-linux-x86_64.zip
    set +x
fi

xmake b -yrD

output=`file build/android/arm64-v8a/release/android-cross` || exit -1
if [[ "$output" =~ "ARM" || "$output" =~ "aarch64" ]]; then
    ok "output file is ARM aarch64"
else
    error "output file is not ARM aarch64!"
    exit 1
fi
