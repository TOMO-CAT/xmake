#!/bin/bash

rm -rf build
rm -rf .xmake
xmake build -r --all || exit 1
