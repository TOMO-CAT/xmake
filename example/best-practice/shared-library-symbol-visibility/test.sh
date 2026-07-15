#!/bin/bash

set -e


xmake config -c
xmake build
xmake install -o build/release
