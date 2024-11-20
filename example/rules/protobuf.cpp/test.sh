#!/bin/bash

xmake b -ry --all || exit -1
xmake run main || exit -1
