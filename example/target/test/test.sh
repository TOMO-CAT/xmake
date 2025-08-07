#!/bin/bash

# export XMAKE_PROGRAM_DIR="/home/k/github/xmake/xmake"
# xrepo env -b emmylua_debugger -v -- xmake b -vrD

# 编译
xmake build --yes --all || exit 1

# 运行单测
xmake run foo_test || exit 1
xmake run old_foo_test || exit 1

# 这三种写法等效
# https://github.com/TOMO-CAT/xmake/issues/207
xmake test foo_test_with_testfiles || exit 1
xmake run foo_test_with_testfiles || exit 1
xmake test foo_test_with_testfiles/default || exit 1
