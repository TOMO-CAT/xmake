#!/bin/bash

runtime="lua"
mode="release"

for arg in "$@"; do
    case $arg in
        --runtime=*)
            runtime="${arg#*=}"
            shift
            ;;
        --mode=*)
            mode="${arg#*=}"
            shift
            ;;
        *)
            echo "Unknown argument: $arg"
            ;;
    esac
done

source "scripts/common.sh"

info "runtime=${runtime} mode=${mode}"
info "start installing xmake ..."

rm -rf build

git submodule update --init || exit 1

./configure --verbose --runtime="${runtime}" --mode="${mode}" || exit 1

# 显示具体编译命令可用 make VERBOSE=1
make || exit 1
sudo make install PREFIX=/usr/local || exit 1

ok "install xmake successfully!"

xmake --version
