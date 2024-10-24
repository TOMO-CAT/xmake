#!/bin/bash

set -e

export CPP="clang++ -E"
export CC="clang"
export CXX="clang++"
export LD="clang++"
export XMAKE_ROOT="y"

# unset CPLUS_INCLUDE_PATH && C_INCLUDE_PATH envs for cross build situation
export CPLUS_INCLUDE_PATH=""
export C_INCLUDE_PATH=""

xmake l -vD tests/run.lua "$@"
