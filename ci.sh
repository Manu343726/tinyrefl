#/bin/sh

mkdir build && cd build
cmake .. -DTINYREFL_BUILD_TESTS=ON -DTINYREFL_BUILD_EXAMPLES=ON
make -j
ctest . -V
./examples/tinyrefl-example
