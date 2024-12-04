#!/bin/bash

exec /usr/lib/llvm-14/bin/llvm-cov gcov --color -f "$@"
