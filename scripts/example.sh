#!/bin/bash

source "scripts/common.sh"

original_dir=$(pwd)

find example -type f -name "test.sh" | while read test_script; do
    script_dir=$(dirname "$test_script")

    info "Running script [$test_script] ..."

    cd "$script_dir" || continue

    set +e
    output=$(bash test.sh 2>&1)

    if [[ $? -ne 0 ]]; then
        error "Script failed in example [$script_dir] with output:"
        echo "$output"
        exit 1
    fi
    set -e

    cd "$original_dir" || exit
done

ok "Run all examples successfully!"
