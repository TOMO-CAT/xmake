#!/bin/bash

set -e

xmake build -v -r -a -y || exit 1
