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

# 交叉编译必须禁用掉对应的环境变量(这相当于环境变量是最高优先级?), 否则会用 host clang++ 以及错误的系统头文件
unset CPLUS_INCLUDE_PATH
unset C_INCLUDE_PATH
unset CPP
unset CC
unset CXX
unset LD

SDK_ZIP_FILE="arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-linux-gnueabihf.tar.xz"
SDK_DIR="arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-linux-gnueabihf"

if [ -d "${SDK_DIR}" ]; then
    warning "dir [${SDK_DIR}] exists, skip installing"
else
    if [ -f "${SDK_ZIP_FILE}" ]; then
        warning "file [${SDK_ZIP_FILE}] exists. skip downloading"
    else
        info "start downloading ndk zip file [${SDK_ZIP_FILE}]"
        wget "https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-linux-gnueabihf.tar.xz" -O ${SDK_ZIP_FILE}
    fi
    tar -xf ${SDK_ZIP_FILE}
fi

# 编译交叉编译版本 (必须用绝对路径, 否则 cmake 会报错找不到 arm-none-linux-gnueabihf-g++ 等二进制)
xmake f -yvD -p cross --sdk=`pwd`/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-linux-gnueabihf
xmake b -yvrD

output=`file build/cross/arm/release/arm-none-linux-gnueabihf-cross` || exit -1
if [[ "$output" =~ "ARM" && "$output" =~ "ELF 32-bit" ]]; then
    ok "output file is ARM 32-bit"
else
    error "output file is not ARM 32-bit!"
    exit 1
fi

# 编译 host 版本
xmake f -cvyD
xmake b -yvrD
output=`file build/linux/x86_64/release/arm-none-linux-gnueabihf-cross` || exit -1
if [[ "$output" =~ "x86-64" ]]; then
    ok "output file is x86_64"
else
    error "output file is not x86_64!"
    exit 1
fi
