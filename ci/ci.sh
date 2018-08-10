#/bin/sh
set -e

if [[ "$CI_RUNNER_TAGS" = *"Manu343726-runner"* ]]; then
    MAKE_CONCURRENCY="-j"
else
    MAKE_CONCURRENCY="-j1"
fi;

if [[ -z "$SOURCE_DIR" ]]; then
    SOURCE_DIR=".."
fi

if [[ -z "$LLVM_VERSION" ]]; then
    LLVM_VERSION="4.0.0"
fi

mkdir build && cd build

echo Cross building: $CROSS_BUILDING
echo Source dir: $SOURCE_DIR
echo LLVM version: $LLVM_VERSION

if [ "$CROSS_BUILDING" == "YES" ]; then
    cmake $SOURCE_DIR -DCMAKE_TOOLCHAIN_FILE="/usr/share/toolchain.cmake" -DTINYREFL_LLVM_VERSION="${LLVM_VERSION}" -DTINYREFL_LLVM_DOWNLOAD_FROM_OFFICIAL_SERVER="ON"
    make $MAKE_CONCURRENCY VERBOSE=1
else
    cmake $SOURCE_DIR -DTINYREFL_LLVM_VERSION="${LLVM_VERSION}" -DTINYREFL_LLVM_DOWNLOAD_FROM_OFFICIAL_SERVER="ON"
    make $MAKE_CONCURRENCY VERBOSE=1
    ctest . -V
    ./examples/tinyrefl-example
fi
