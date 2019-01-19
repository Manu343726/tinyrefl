PROJECT(TinyreflExample)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

CONAN_BASIC_SETUP(TARGETS)

add_executable(tinyrefl_example main.cpp)

find_package(tinyrefl_tool REQUIRED)
tinyrefl_tool(TARGET tinyrefl_example HEADERS example.hpp)
