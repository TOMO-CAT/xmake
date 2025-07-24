#!/bin/bash

export XMAKE_PROGRAM_DIR="/home/k/github/xmake/xmake"

xrepo env -b emmylua_debugger -v -- xmake b -vrD
