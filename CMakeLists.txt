cmake_minimum_required(VERSION 3.0)
project(tinyrefl)

set(CPPAST_BUILD_EXAMPLE OFF)
set(CPPAST_BUILD_TEST OFF)
set(CPPAST_BUILD_TOOL OFF)
set(CXXOPTS_BUILD_EXAMPLES OFF)

set(CMAKE_CXX_STANDARD 14)
add_subdirectory(fmt)
add_subdirectory(cppast)
add_subdirectory(ctti)
add_subdirectory(masquerade)
add_subdirectory(src)

add_executable(tool tool.cpp)
add_executable(example example.cpp)
target_link_libraries(tool    PRIVATE tinyrefl)
target_link_libraries(example PRIVATE tinyrefl)

configure_file(
    ${CMAKE_SOURCE_DIR}/.ycm_extra_conf.py.in
    ${CMAKE_SOURCE_DIR}/.ycm_extra_conf.py
)
