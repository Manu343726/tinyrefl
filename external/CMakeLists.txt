include(${TINYREFL_SOURCE_DIR}/utils.cmake)

find_package(Git REQUIRED)

execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive)

set(CPPAST_BUILD_EXAMPLE OFF CACHE BOOL "disable cppast examples")
set(CPPAST_BUILD_TEST OFF CACHE BOOL "disable cppast tests")
set(CPPAST_BUILD_TOOL OFF CACHE BOOL "disable cppast tool")

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
    set(LLVM_DOWNLOAD_URL "https://dl.bintray.com/manu343726/llvm-releases/clang+llvm-${TINYREFL_LLVM_VERSION}-x86_64-linux-gnu-ubuntu-14.04.tar.xz")
endif()

add_subdirectory(cppast)
add_subdirectory(ctti)
add_subdirectory(fmt)
add_subdirectory(masquerade)
add_subdirectory(cppfs)
