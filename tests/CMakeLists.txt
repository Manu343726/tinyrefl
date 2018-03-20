include(${TINYREFL_SOURCE_DIR}/tool/driver.cmake)

add_subdirectory(static)

add_executable(tinyrefl-test
    main.cpp
    api.cpp
)
target_link_libraries(tinyrefl-test PRIVATE tinyrefl)
tinyrefl_tool(TARGET tinyrefl-test HEADERS example.hpp)
add_test(tinyrefl-test tinyrefl-test)
