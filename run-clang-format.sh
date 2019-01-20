#!/bin/bash

CLANG_FORMAT=$1
DIRECTORIES=(include src tests examples)

root_dir=$(pwd)

if [[ -z "$CLANG_FORMAT" ]]; then
    CLANG_FORMAT="docker run --rm -v ${root_dir}:${root_dir} unibeautify/clang-format"
fi

echo CLANG_FORMAT=$CLANG_FORMAT

for dir in ${DIRECTORIES[@]}; do
    files=$(find "${root_dir}/${dir}" -iname "*.hpp" -o -iname "*.cpp")
    echo formatting $files
    $CLANG_FORMAT -i -style=file $files
done
