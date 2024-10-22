#!/bin/bash

source "scripts/common.sh"

info "start installing xmake ..."

git submodule update --init
./configure
make
sudo make install PREFIX=/usr/local

ok "install xmake successfully!"

xmake --version
