#!/bin/bash

# export XMAKE_PROGRAM_DIR="/home/k/github/xmake/xmake"
# xrepo env -b emmylua_debugger -v -- xmake b -vrD

# 编译
xmake build --yes --all || exit 1

# 运行单测
xmake run foo_test || exit 1
xmake run old_foo_test || exit 1
