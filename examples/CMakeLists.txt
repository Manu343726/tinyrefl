include(${TINYREFL_SOURCE_DIR}/tool/driver.cmake)
add_executable(tinyrefl-example example.cpp)
tinyrefl_tool(TARGET tinyrefl-example HEADERS example.hpp)
