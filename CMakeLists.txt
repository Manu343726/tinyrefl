cmake_minimum_required(VERSION 3.11.4)

set(TINYREFL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "")
set(TINYREFL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "")
set(TINYREFL_INCLUDE_DIR "${TINYREFL_SOURCE_DIR}/include" CACHE PATH "")

list(APPEND CMAKE_PREFIX_PATH "${TINYREFL_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${TINYREFL_BINARY_DIR}")

include(cmake/git.cmake)
project(tinyrefl VERSION "${TINYREFL_VERSION}" LANGUAGES CXX)

include(cmake/utils.cmake)
include(cmake/config.cmake)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(TINYREFL_IS_ROOT_PROJECT TRUE)
else()
    set(TINYREFL_IS_ROOT_PROJECT FALSE)
endif()

option(TINYREFL_USE_LOCAL_LLVM "Use local LLVM install instead of downloading LLVM" OFF)
option(TINYREFL_LLVM_DOWNLOAD_FROM_OFFICIAL_SERVER "Download LLVM from official LLVM server instead of a mirror" OFF)
option(TINYREFL_BUILD_TESTS "Build tinyrefl tests" ${TINYREFL_IS_ROOT_PROJECT})
option(TINYREFL_BUILD_EXAMPLES "Build tinyrefl examples" ${TINYREFL_IS_ROOT_PROJECT})
option(TINYREFL_BUILD_TOOL "Build tinyrefl-tool" ON)

if(NOT DEFINED TINYREFL_LLVM_VERSION)
    set(TINYREFL_LLVM_VERSION "6.0.1")
endif()
parse_version_number(${TINYREFL_LLVM_VERSION} TINYREFL_LLVM_VERSION_MAJOR TINYREFL_LLVM_VERSION_MINOR TINYREFL_LLVM_VERSION_FIX)

if(CMAKE_TOOLCHAIN_FILE OR TINYREFL_CROSS_BUILDING)
    message(STATUS "Tinyrefl cross building")
    set(TINYREFL_CROSS_BUILDING ON CACHE BOOL "")
else()
    message(STATUS "Tinyrefl building for host")
    set(TINYREFL_CROSS_BUILDING OFF CACHE BOOL "")
endif()

set(CONAN_BUILDINFO_FILE "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
if(EXISTS "${CONAN_BUILDINFO_FILE}")
    message(STATUS "Importing Conan targets from build info file...")
    include("${CONAN_BUILDINFO_FILE}")
    conan_basic_setup(TARGETS)
    set(TINYREFL_USING_CONAN_TARGETS TRUE CACHE INTERNAL "")

    if(NOT MSVC)
        if(CONAN_SETTINGS_COMPILER_LIBCXX STREQUAL "libstdc++11")
            add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
        endif()
        if(CONAN_SETTINGS_COMPILER_LIBCXX STREQUAL "libstdc++")
            add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
        endif()
    endif()
endif()

message(STATUS "Configuring tinyrefl external dependencies...")
include(external/external.cmake)
message(STATUS "Configuring tinyrefl library...")
add_subdirectory(src)

if(TINYREFL_BUILD_TOOL)
    message(STATUS "Configuring tinyrefl tool...")
    add_subdirectory(tool)
endif()

# Implicitly add driver for users
include(${TINYREFL_SOURCE_DIR}/tool/driver.cmake)

if(TINYREFL_BUILD_TESTS AND NOT TINYREFL_CROSS_BUILDING)
    include(cmake/ctest_setup.cmake)
    message(STATUS "Configuring tinyrefl tests...")
    add_subdirectory(tests)
endif()

if(TINYREFL_BUILD_EXAMPLES)
    include(cmake/ctest_setup.cmake)
    message(STATUS "Configuring tinyrefl examples...")
    add_subdirectory(examples)
endif()

if(TINYREFL_IS_ROOT_PROJECT)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

    message(STATUS "Generate vim YCM config file for tinyrefl at ${TINYREFL_SOURCE_DIR} ...")
    configure_file(
        ${TINYREFL_SOURCE_DIR}/.ycm_extra_conf.py.in
        ${TINYREFL_SOURCE_DIR}/.ycm_extra_conf.py
    )
endif()
