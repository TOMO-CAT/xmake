#!/bin/bash

set -e

BASE_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")";pwd)

# 需要删除之前的编译产物, 否则会报错
# rm -rf build
# https://stackoverflow.com/questions/47393377/annoying-error-message-cannot-merge-previous-gcda-file
find . -name "*.gcda" -print0 | xargs -0 rm -f

xmake f -y -m coverage
xmake b
xmake test -v

rm -rf coverage
mkdir -p coverage

# apt install lcov
lcov --rc lcov_branch_coverage=1 --directory build/.objs --base-directory . --gcov-tool "${BASE_DIR}/llvm-gcov.sh" --capture -o coverage/coverage.info
lcov --rc lcov_branch_coverage=1 --extract coverage/coverage.info "${BASE_DIR}/src/*" --output-file coverage/filtered_coverage.info

genhtml coverage/filtered_coverage.info --highlight --legend --branch-coverage --output-directory coverage

echo 'coverage report has already been generated, and it can be viewed with the following command: "(cd coverage && python3 -m http.server 17777)"'
