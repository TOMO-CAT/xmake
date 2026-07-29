#!/usr/bin/env bash

set -e

chmod +x slow-clangxx.sh
rm -rf .xmake build
xmake f -c -y -m release --toolchain=repro-clangxx --verbose --diagnosis
xmake -r -y --verbose --diagnosis
