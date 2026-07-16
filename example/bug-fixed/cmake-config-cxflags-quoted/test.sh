#!/usr/bin/env bash

set -e

rm -rf .xmake build package/build

xmake config \
    --plat=cross \
    --arch=x86_64 \
    --toolchain=repro-clang \
    --cxflags="-flto=full -fPIC" \
    --verbose \
    --diagnosis \
    --yes
