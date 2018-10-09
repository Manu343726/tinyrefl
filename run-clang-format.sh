#!/bin/bash

CLANG_FORMAT=$1
DIRECTORIES=(include src tests examples)

if [[ -z "$CLANG_FORMAT" ]]; then
    CLANG_FORMAT=./clang-format
fi

echo CLANG_FORMAT=$CLANG_FORMAT
$CLANG_FORMAT --version

for dir in ${DIRECTORIES[@]}; do
    for file in $(find $dir -iname "*.hpp" -o -iname "*.cpp"); do
        echo formatting $file
        $CLANG_FORMAT -i -style=file $file
    done
done
