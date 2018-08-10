cmake_minimum_required(VERSION 3.0)
project(tinyrefl)

include (CheckCXXCompilerFlag)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(TINYREFL_IS_ROOT_PROJECT TRUE)
else()
    set(TINYREFL_IS_ROOT_PROJECT FALSE)
endif()

option(TINYREFL_USE_LOCAL_LLVM "Use local LLVM install instead of downloading LLVM" OFF)
option(TINYREFL_LLVM_DOWNLOAD_FROM_OFFICIAL_SERVER "Download LLVM from official LLVM server instead of artifactory" OFF)
option(TINYREFL_BUILD_TESTS "Build tinyrefl tests" ${TINYREFL_IS_ROOT_PROJECT})
option(TINYREFL_BUILD_EXAMPLES "Build tinyrefl examples" ${TINYREFL_IS_ROOT_PROJECT})

if(NOT DEFINED TINYREFL_LLVM_VERSION)
    set(TINYREFL_LLVM_VERSION "4.0.0")
endif()

set(CMAKE_CXX_STANDARD 14)

if(NOT MSVC)
    check_cxx_compiler_flag("-Wnoexcept-type" HAS_NOEXCEPT_ABI_WARNING)

    if(HAS_NOEXCEPT_ABI_WARNING)
        set(no_noexcept_abi_warning "-Wno-noexcept-type")
    endif()

    set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-attributes ${no_noexcept_abi_warning}")
endif()

set(TINYREFL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "")
set(TINYREFL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "")
set(TINYREFL_INCLUDE_DIR "${TINYREFL_SOURCE_DIR}/include" CACHE PATH "")

if(CMAKE_TOOLCHAIN_FILE OR TINYREFL_CROSS_BUILDING)
    message(STATUS "Tinyrefl cross building")
    set(TINYREFL_CROSS_BUILDING ON CACHE BOOL "")
else()
    message(STATUS "Tinyrefl building for host")
    set(TINYREFL_CROSS_BUILDING OFF CACHE BOOL "")
endif()

message(STATUS "Configuring tinyrefl external dependencies...")
add_subdirectory(external)
message(STATUS "Configuring tinyrefl library...")
add_subdirectory(src)
message(STATUS "Configuring tinyrefl tool...")
add_subdirectory(tool)

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
    message(STATUS "Generate vim YCM config file for tinyrefl at ${TINYREFL_SOURCE_DIR} ...")
    configure_file(
        ${TINYREFL_SOURCE_DIR}/.ycm_extra_conf.py.in
        ${TINYREFL_SOURCE_DIR}/.ycm_extra_conf.py
    )
endif()
