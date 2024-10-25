#!/bin/bash

set -e

export CPP="clang++ -E"
export CC="clang"
export CXX="clang++"

# export LD="clang++"
# 
# install libtool fail
# https://github.com/conan-io/conan-center-index/issues/25251
unset LD

export XMAKE_ROOT="y"

# unset CPLUS_INCLUDE_PATH && C_INCLUDE_PATH envs for cross build situation
export CPLUS_INCLUDE_PATH=""
export C_INCLUDE_PATH=""

xmake l -vD tests/run.lua "$@"
