include(${TINYREFL_SOURCE_DIR}/tool/driver.cmake)

add_executable(tinyrefl-test-static
    main.cpp
    backend.cpp
    generated_code.cpp
)

target_link_libraries(tinyrefl-test-static PRIVATE tinyrefl)
target_include_directories(tinyrefl-test-static PRIVATE "${CMAKE_SOURCE_DIR}/external")

tinyrefl_tool(TARGET tinyrefl-test-static
HEADERS
    strings.hpp
    members.hpp
)
