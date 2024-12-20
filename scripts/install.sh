#!/bin/bash

runtime=""

for arg in "$@"; do
    case $arg in
        --runtime=*)
            runtime="${arg#*=}"
            shift
            ;;
        *)
            echo "Unknown argument: $arg"
            ;;
    esac
done

source "scripts/common.sh"

info "start installing xmake ..."

rm -rf build

git submodule update --init || exit 1

if [ -n "$runtime" ]; then
    ./configure --verbose --runtime=luajit || exit 1
else
    ./configure --verbose || exit 1
fi

make || exit 1
sudo make install PREFIX=/usr/local || exit 1

ok "install xmake successfully!"

xmake --version
