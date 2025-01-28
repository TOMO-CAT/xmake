#!/bin/bash

set -e

# trace 宏依赖 debug 模式
xmake f -m debug --ut=y
xmake b -vD --all

# 运行单测
xmake run xutil-ut memory_buffer
xmake run xutil-ut memory_check
xmake run xutil-ut utils_trace
