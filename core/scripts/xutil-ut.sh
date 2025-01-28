#!/bin/bash

# trace 宏依赖 debug 模式
xmake f -m debug --ut=y

# 运行单测
xmake run xutil-ut utils_trace
