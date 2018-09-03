cmake_minimum_required(VERSION 3.0.0)
project(TinyreflUserProject)

set(CMAKE_CXX_STANDARD 14)
set(TINYREFL_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../..")
set(TINYREFL_BINARY_DIR "${CMAKE_BINARY_DIR}/tinyrefl")

if(NOT EXISTS "${TINYREFL_BINARY_DIR}")
    make_directory("${TINYREFL_BINARY_DIR}")
endif()
add_subdirectory("${TINYREFL_SOURCE_DIR}" "${TINYREFL_BINARY_DIR}")

if(NOT TARGET tinyrefl)
    message(FATAL_ERROR "tinyrefl target not found")
else()
    message(STATUS "tinyrefl include dir: ${TINYREFL_INCLUDE_DIR}")
endif()

add_library(userapi userapi.cpp)
tinyrefl_tool(TARGET userapi HEADERS userapi.hpp)

enable_testing()
add_executable(userapitests userapitests.cpp)
target_link_libraries(userapitests PRIVATE userapi)
add_test(userapitests userapitests)
