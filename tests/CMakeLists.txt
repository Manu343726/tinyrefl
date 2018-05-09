include(${TINYREFL_SOURCE_DIR}/tool/driver.cmake)

add_subdirectory(static)

add_tinyrefl_test(tinyrefl-test
    main.cpp
    api.cpp
)
target_link_libraries(tinyrefl-test PRIVATE tinyrefl)
tinyrefl_tool(TARGET tinyrefl-test HEADERS example.hpp)
