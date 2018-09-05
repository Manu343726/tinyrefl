#!/bin/bash

CLANG_FORMAT=$1
DIRECTORIES=(include src tests examples)

if [[ -z "$CLANG_FORMAT" ]]; then
    CLANG_FORMAT=clang-format
fi

export -f format_file

echo CLANG_FORMAT=$CLANG_FORMAT

for dir in ${DIRECTORIES[@]}; do
    for file in $(find $dir -iname "*.hpp" -o -iname "*.cpp"); do
        echo formatting $file
        $CLANG_FORMAT -i -style=file $file
    done
done
