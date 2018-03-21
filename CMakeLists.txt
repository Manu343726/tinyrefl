cmake_minimum_required(VERSION 3.0)
project(tinyrefl)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-inline")
set(TINYREFL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "")
set(TINYREFL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "")
set(TINYREFL_INCLUDE_DIR "${TINYREFL_SOURCE_DIR}/include" CACHE PATH "")

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(tool)
add_subdirectory(examples)

option(TINYREFL_BUILD_TESTS "Build unit tests" OFF)
if(TINYREFL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(STATUS "Generate vim YCM config file for tinyrefl at ${TINYREFL_SOURCE_DIR} ...")
    configure_file(
        ${TINYREFL_SOURCE_DIR}/.ycm_extra_conf.py.in
        ${TINYREFL_SOURCE_DIR}/.ycm_extra_conf.py
    )
endif()
