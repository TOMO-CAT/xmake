#!/bin/bash

set -e

cd "$(dirname "$0")"
xmake lua test.lua
