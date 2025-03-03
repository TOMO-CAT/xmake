#!/bin/bash

runtime="lua"
mode="release"
verbose=0

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
        --verbose)
            verbose=1
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

if [[ $verbose -eq 1 ]]; then
    ./configure --verbose --runtime="${runtime}" --mode="${mode}" || exit 1
    make VERBOSE=1 || exit 1
else
    ./configure --verbose --runtime="${runtime}" --mode="${mode}" || exit 1
    make || exit 1
fi

sudo make install PREFIX=/usr/local || exit 1

ok "install xmake successfully!"

xmake --version
