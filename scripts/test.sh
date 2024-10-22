#!/bin/bash

set -e

export CPP="clang++ -E"
export CC="clang"
export CXX="clang++"
export LD="clang++"

xmake l -vD tests/run.lua "$@"
