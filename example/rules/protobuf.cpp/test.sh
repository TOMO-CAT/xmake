#!/bin/bash

xmake -br --all || exit -1
xmake run main || exit -1
