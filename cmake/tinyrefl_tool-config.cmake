include(FindPackageHandleStandardArgs)

set(TINYREFL_PACKAGE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
set(TINYREFL_PACKAGE_CMAKE_DIR "${TINYREFL_PACKAGE_DIR}/cmake")
set(TINYREFL_PACKAGE_BIN_DIR "${TINYREFL_PACKAGE_DIR}/bin")
set(TINYREFL_TOOL_EXECUTABLE_FILENAME tinyrefl-tool)
set(__TINYREFL_TOOL_EXECUTABLE_EXPECTED_PATH "${TINYREFL_PACKAGE_DIR}/bin/${TINYREFL_TOOL_EXECUTABLE_FILENAME}")

if(CONAN_BIN_DIRS_CLANG_EXECUTABLES)
    message(STATUS "tinyrefl_tool config using clang binaries from conan")
    find_program(clangpp_executable clang++ PATHS "${CONAN_BIN_DIRS_CLANG_EXECUTABLES}" NO_DEFAULT_PATH)
else()
    message(STATUS "tinyrefl_tool config will search system clang binaries")
    find_program(clangpp_executable clang++)
endif()

if(clangpp_executable)
    set(TINYREFL_TOOL_CLANGPP_EXECUTABLE "${clangpp_executable}")
    message(STATUS "tinyrefl_tool config found clang++: ${TINYREFL_TOOL_CLANGPP_EXECUTABLE}")
endif()

if(EXISTS "${__TINYREFL_TOOL_EXECUTABLE_EXPECTED_PATH}")
    set(TINYREFL_TOOL_EXECUTABLE "${__TINYREFL_TOOL_EXECUTABLE_EXPECTED_PATH}")
    set(TINYREFL_SOURCE_DIR "${TINYREFL_PACKAGE_DIR}")
    list(APPEND CMAKE_MODULE_PATH "${TINYREFL_PACKAGE_CMAKE_DIR}")
    include("${TINYREFL_PACKAGE_CMAKE_DIR}/driver.cmake")
endif()

find_package_handle_standard_args(tinyrefl_tool REQUIRED_VARS TINYREFL_TOOL_EXECUTABLE TINYREFL_SOURCE_DIR)
