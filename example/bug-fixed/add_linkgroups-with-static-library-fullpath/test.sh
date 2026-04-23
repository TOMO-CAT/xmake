#!/bin/bash

set -e

xmake build -r -a -y || exit 1
