#!/bin/bash

xmake -bvD --all || exit -1
xmake run main || exit -1
