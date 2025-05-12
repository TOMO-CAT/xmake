#!/bin/bash

set -e

# bash scripts/busted.sh

export XMAKE_PROGRAM_DIR="${PWD}/xmake"

export XMAKE_ROOT="y"

# export CPP="clang++ -E"
# export CC="clang"
# export CXX="clang++"

# install libtool fail
#
# https://github.com/conan-io/conan-center-index/issues/25251

# tests/projects/linux/bpf/minimal/test.lua
#
# configure: error: llvm-profdata is required for a --enable-optimizations build but could not be found.
#
# https://github.com/python/cpython/issues/80183
export LLVM_PROFDATA="/usr/lib/llvm-14/bin/llvm-profdata"

# /usr/bin/ld: cannot find /usr/lib/llvm-14/lib/clang/14.0.6/lib/linux/libclang_rt.profile-x86_64.a: No such file or directory
#
# http://logan.tw/posts/2015/04/28/check-code-coverage-with-clang-and-lcov/
# sudo apt install libclang-rt-14-dev

# tests/projects/c++/snippet_runtimes/test.lua
#
# /usr/bin/ld: cannot find -lc++abi: No such file or directory
#
# sudo apt-get install libc++abi-dev

path="$1"

# Remove 'tests/' from the beginning of the path
if [[ "$path" == tests/* ]]; then
    path="${path#tests/}"
fi

# Remove '/test.lua' from the end of the path
if [[ "$path" == */test.lua ]]; then
    path="${path%/test.lua}"
fi

# Remove '/xmake.lua' from the end of the path
if [[ "$path" == */xmake.lua ]]; then
    path="${path%/xmake.lua}"
fi

xmake lua --verbose --diagnosis tests/run.lua "$path"
