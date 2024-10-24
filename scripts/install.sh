#!/bin/bash

source "scripts/common.sh"

info "start installing xmake ..."

rm -rf build

git submodule update --init
./configure
make
sudo make install PREFIX=/usr/local

ok "install xmake successfully!"

xmake --version
