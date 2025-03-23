#!/bin/bash

source "scripts/common.sh"

original_dir=$(pwd)

export XMAKE_PROGRAM_DIR="${PWD}/xmake"

mapfile -t test_scripts < <(find example -type f -name "test.sh")

if [ -z "${GITHUB_ACTIONS+x}" ]; then
    info "not running in github actions"
    SHOW_OUTPUT=true
else
    info "running in github actions, we will show all output"
    SHOW_OUTPUT=true
fi

declare -a timings

for test_script in "${test_scripts[@]}"; do
    script_dir=$(dirname "$test_script")

    info "Running script [$test_script] ..."

    cd "$script_dir" || continue
    start_time=$(date +%s)

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

    end_time=$(date +%s)
    duration=$((end_time - start_time))
    timings+=("$duration:$script_dir")

    cd "$original_dir" || exit
done

info "Timings for each example (sorted by duration, descending):"
echo "-----------------------------------------------------------------"
printf "%-10s %s\n" "Duration" "Example"
echo "-----------------------------------------------------------------"
for timing in $(printf "%s\n" "${timings[@]}" | sort -nr); do
    duration=$(echo "$timing" | cut -d: -f1)
    example=$(echo "$timing" | cut -d: -f2)
    printf "%-10s %s\n" "${duration}s" "$example"
done
echo "-----------------------------------------------------------------"

ok "Run all examples successfully!"
