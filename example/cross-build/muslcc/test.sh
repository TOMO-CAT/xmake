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

xmake f -cvyD
xmake b -yvrD
