include(${TINYREFL_SOURCE_DIR}/tool/driver.cmake)

add_subdirectory(static)

add_tinyrefl_test(tinyrefl-test
    main.cpp
    api.cpp
    driver_handles_headers_in_different_dirs.cpp
    example.cpp
)
target_link_libraries(tinyrefl-test PRIVATE tinyrefl)
#target_compile_options(tinyrefl-test PRIVATE -E)
target_compile_options(tinyrefl-test PRIVATE ${TINYREFL_CXX_WARNINGS})

tinyrefl_tool(TARGET tinyrefl-test
HEADERS
    example.hpp
    dummy_folder/dummy_header.hpp
)

add_dependencies(tinyrefl-test-static tinyrefl-test)
