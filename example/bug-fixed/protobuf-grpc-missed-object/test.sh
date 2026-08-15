#!/bin/sh

set -e
xmake f --ccache=n -y
xmake b -v -r
