#!/bin/bash

set -e

function skip() {
  (>&2 printf "[\e[35m\e[1mSKIP\e[0m] $*\n")
}

# 依赖特定版本的 llvm-cov（与 llvm-gcov.sh 配套），不存在则跳过
LLVM_COV_PATH="/usr/lib/llvm-14/bin/llvm-cov"
if [ ! -f "${LLVM_COV_PATH}" ]; then
  skip "${LLVM_COV_PATH} not found, skip this test (need llvm-14)"
  exit 0
fi

# 同时清空 build 和 build_cache 运行往往正常
rm -rf build
rm -rf ~/.xmake/.build_cache
bash coverage.sh

# 只删除 build 而不删除 build_cache 运行则报错
rm -rf build
bash coverage.sh
