#!/bin/bash

# 设置环境变量, 支持调试 xmake example 和 tests
# 用法: source scripts/env.sh

export XMAKE_PROGRAM_DIR="${PWD}/xmake"

echo ""
echo -e "  \033[1;36mTip:\033[0m 使用以下命令可以进行 Lua 代码调试:"
echo -e "  \033[1;33mxrepo env -b emmylua_debugger -v -- xmake b -vrD\033[0m"
echo ""
