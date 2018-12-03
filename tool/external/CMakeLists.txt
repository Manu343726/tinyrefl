include(${TINYREFL_SOURCE_DIR}/cmake/utils.cmake)
include(${TINYREFL_SOURCE_DIR}/cmake/externals.cmake)

set(CPPAST_BUILD_EXAMPLE OFF CACHE BOOL "disable cppast examples")
set(CPPAST_BUILD_TEST OFF CACHE BOOL "disable cppast tests")
set(CPPAST_BUILD_TOOL OFF CACHE BOOL "disable cppast tool")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "build cppfs as static lib")
set(OPTION_BUILD_TESTS OFF CACHE BOOL "disable cppfs tests")

parse_version_number(${TINYREFL_LLVM_VERSION} TINYREFL_LLVM_VERSION_MAJOR TINYREFL_LLVM_VERSION_MINOR TINYREFL_LLVM_VERSION_FIX)

if(TINYREFL_USE_LOCAL_LLVM)
    find_program(llvm-config NAMES llvm-config llvm-config-${TINYREFL_LLVM_VERSION_MAJOR}.${TINYREFL_LLVM_VERSION_MINOR})

    if(llvm-config)
        execute_process(COMMAND ${llvm-config} --version
            OUTPUT_VARIABLE llvm-config-version OUTPUT_STRIP_TRAILING_WHITESPACE)

        if(llvm-config-version VERSION_EQUAL TINYREFL_LLVM_VERSION)
            set(LLVM_CONFIG_BINARY "${llvm-config}")
            message(STATUS "Using local LLVM ${TINYREFL_LLVM_VERSION} install")
        else()
            message(FATAL_ERROR "Wrong LLVM install found. Found llvm-config ${llvm-config-version}, required ${TINYREFL_LLVM_VERSION}")
        endif()
    else()
        message(FATAL_ERROR "TINYREFL_USE_LOCAL_LLVM set and llvm-config program not found")
    endif()
else()
    if(NOT TINYREFL_LLVM_DOWNLOAD_URL)
        message(STATUS "Using default LLVM download url from bintray")
        set(TINYREFL_LLVM_DOWNLOAD_URL "https://dl.bintray.com/manu343726/llvm-releases/clang+llvm-${TINYREFL_LLVM_VERSION}-x86_64-linux-gnu-ubuntu-14.04.tar.xz")
    else()
        message(STATUS "Using custom LLVM download url: ${TINYREFL_LLVM_DOWNLOAD_URL}")
    endif()

    # LLVM releases are compiled with old GCC ABI
    if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
        add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
    endif()

    set(LLVM_DOWNLOAD_URL "${TINYREFL_LLVM_DOWNLOAD_URL}")
endif()

external_dependency(fmt https://github.com/fmtlib/fmt.git 2b2cfdac1929b2c10a83cdff3884e4d974f4aac3)
external_dependency(cppast https://github.com/Manu343726/cppast.git bytech)
external_dependency(cppfs https://github.com/Manu343726/cppfs.git optional_libSSL2)
external_dependency(masquerade https://gitlab.com/Manu343726/masquerade.git 189afd15c1fa1cf97fce0adc6efb6cb3e4efad65)

if(NOT LLVM_CONFIG_BINARY)
    message(FATAL_ERROR "llvm-config binary not set")
else()
    message(STATUS "llvm-config binary: ${LLVM_CONFIG_BINARY}")
endif()

execute_process(COMMAND ${LLVM_CONFIG_BINARY} --libdir OUTPUT_VARIABLE stdout)
string(STRIP "${stdout}" stdout)
set(LLVM_CMAKE_PATH "${stdout}/cmake/llvm" CACHE PATH "")
set(CLANG_CMAKE_PATH "${stdout}/cmake/clang" CACHE PATH "")

message(STATUS "llvm cmake path: ${LLVM_CMAKE_PATH}")
message(STATUS "clang cmake path: ${CLANG_CMAKE_PATH}")
