cmake_minimum_required(VERSION 3.0)
project(tinyrefl)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-inline")
set(TINYREFL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(TINYREFL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(TINYREFL_INCLUDE_DIR "${TINYREFL_SOURCE_DIR}/include")

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(tool)
add_subdirectory(examples)

option(TINYREFL_BUILD_TESTS "Build unit tests" OFF)
if(TINYREFL_BUILD_TESTS)
    add_subdirectory(tests)
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/.ycm_extra_conf.py.in
    ${CMAKE_SOURCE_DIR}/.ycm_extra_conf.py
)
