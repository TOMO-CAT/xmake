#!/bin/bash

source "scripts/common.sh"

info "start installing xmake ..."

rm -rf build

git submodule update --init || exit 1
./configure --verbose --runtime=luajit || exit 1
make || exit 1
sudo make install PREFIX=/usr/local || exit 1

ok "install xmake successfully!"

xmake --version
