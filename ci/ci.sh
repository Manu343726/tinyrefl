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

mkdir build && cd build

echo Cross building: $CROSS_BUILDING
echo Source dir: $SOURCE_DIR

if [ "$CROSS_BUILDING" == "YES" ]; then
    cmake $SOURCE_DIR -DCMAKE_TOOLCHAIN_FILE="/usr/share/toolchain.cmake" -DTINYREFL_BUILD_EXAMPLES=ON
    make $MAKE_CONCURRENCY VERBOSE=1
else
    cmake $SOURCE_DIR -DTINYREFL_BUILD_TESTS=ON -DTINYREFL_BUILD_EXAMPLES=ON
    make $MAKE_CONCURRENCY VERBOSE=1
    ctest . -V
    ./examples/tinyrefl-example
fi
