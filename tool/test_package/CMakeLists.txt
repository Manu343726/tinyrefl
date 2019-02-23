PROJECT(TinyreflToolExample)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

CONAN_BASIC_SETUP(TARGETS)

message(STATUS "CONAN_BIN_DIRS_CLANG_EXECUTABLES: ${CONAN_BIN_DIRS_CLANG_EXECUTABLES}")

add_library(tinyrefl_tool_example dummy.cpp)

find_package(tinyrefl_tool REQUIRED)
tinyrefl_tool(TARGET tinyrefl_tool_example HEADERS example.hpp)
