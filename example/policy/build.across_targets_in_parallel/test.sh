#!/bin/bash

rm -rf build
rm -rf .xmake
xmake build -r --all -v || exit 1
