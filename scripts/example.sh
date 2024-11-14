#!/bin/bash

source "scripts/common.sh"

original_dir=$(pwd)

find example -type f -name "test.sh" | while read test_script; do
    script_dir=$(dirname "$test_script")
    cd "$script_dir" || continue
    bash test.sh

    cd "$original_dir" || exit
done
