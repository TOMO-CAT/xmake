#!/bin/bash

set -e

# 同时清空 build 和 build_cache 运行往往正常
rm -rf build
rm -rf ~/.xmake/.build_cache
bash coverage.sh

# 只删除 build 而不删除 build_cache 运行则报错
# rm -rf build
# bash coverage.sh
