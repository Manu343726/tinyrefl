#!/bin/bash

set -e

if [[ -z "$CROSS_C_COMPILER" ]]; then
    echo "Using \$CC $CC as CROSS_C_COMPILER"
    CROSS_C_COMPILER=$CC
fi

if [[ -z "$CROSS_CXX_COMPILER" ]]; then
    echo "Using \$CXX $CXX as CROSS_CXX_COMPILER"
    CROSS_CXX_COMPILER=$CXX
fi

TOOLCHAIN_FILE="/usr/share/toolchain.cmake"
echo Generating toolchain file $TOOLCHAIN_FILE ...
echo using CROSS_C_COMPILER=$CROSS_C_COMPILER
echo using CROSS_CXX_COMPILER=$CROSS_CXX_COMPILER

scape_path() {
    echo "${1//\//\\\/}"
}

sed -i "s/\$CROSS_C_COMPILER/$(scape_path $CROSS_C_COMPILER)/g" $TOOLCHAIN_FILE
sed -i "s/\$CROSS_CXX_COMPILER/$(scape_path $CROSS_CXX_COMPILER)/g" $TOOLCHAIN_FILE

echo Output toolchain file:
cat $TOOLCHAIN_FILE
