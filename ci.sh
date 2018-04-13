#/bin/sh
set -e

if [[ "$CI_RUNNER_TAGS" = *"Manu343726-runner"* ]]; then
    MAKE_CONCURRENCY="-j"
else
    MAKE_CONCURRENCY="-j1"
fi;

mkdir build && cd build
cmake .. -DTINYREFL_BUILD_TESTS=ON -DTINYREFL_BUILD_EXAMPLES=ON
make $MAKE_CONCURRENCY
ctest . -V
./examples/tinyrefl-example
