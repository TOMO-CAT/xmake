#!/bin/bash

BASE_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")";pwd)
WORKSPACE=$(realpath "${BASE_DIR}/..")

export LUA_PATH="${WORKSPACE}/xmake/core/?.lua;$LUA_PATH"

busted xmake
