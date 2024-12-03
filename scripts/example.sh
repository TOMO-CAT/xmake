#!/bin/bash

source "scripts/common.sh"

original_dir=$(pwd)

export XMAKE_PROGRAM_DIR="${PWD}/xmake"

mapfile -t test_scripts < <(find example -type f -name "test.sh")

if [ -z "${GITHUB_ACTIONS+x}" ]; then
    info "not running in github actions"
    SHOW_OUTPUT=false
else
    info "running in github actions, we will show all output"
    SHOW_OUTPUT=true
fi

for test_script in "${test_scripts[@]}"; do
    script_dir=$(dirname "$test_script")

    info "Running script [$test_script] ..."

    cd "$script_dir" || continue

    if [ "${SHOW_OUTPUT}" == true ]; then
        echo "-----------------------------------------------------------------"
        bash test.sh
        if [[ $? -ne 0 ]]; then
            error "Script failed in example [$script_dir]"
            exit 1
        fi
        echo "-----------------------------------------------------------------"
    else
        output=$(bash test.sh 2>&1)
        if [[ $? -ne 0 ]]; then
            error "Script failed in example [$script_dir] with output:"
            echo "-----------------------------------------------------------------"
            echo "$output"
            echo "-----------------------------------------------------------------"
            exit 1
        fi
    fi

    cd "$original_dir" || exit
done

ok "Run all examples successfully!"
