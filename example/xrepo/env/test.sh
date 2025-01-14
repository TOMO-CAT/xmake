#!/bin/bash

set -exu

# LuaJIT 2.1.0-beta3 -- Copyright (C) 2005-2017 Mike Pall. http://luajit.org/
xrepo env luajit -v

# 可以通过 --quiet 屏蔽掉 xmake 自身的输出
xrepo env --quiet luajit -v
