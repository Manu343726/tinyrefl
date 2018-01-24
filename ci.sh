#/bin/sh
git submodule update --init --recursive

SOURCE_DIR=$(pwd)
cd cppast/external/type_safe
git apply -v $SOURCE_DIR/stdlibcxx_checks.patch && git diff
cd $SOURCE_DIR

mkdir build && cd build
cmake .. -DLLVM_CONFIG_BINARY="$(which llvm-config-5.0)"
make tool VERBOSE=1 && ./tool ../example.hpp && make && ./example
