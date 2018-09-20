add_tinyrefl_test(tinyrefl-tool-tests
    main.cpp
    driver_handles_headers_in_different_dirs.cpp
    jinja2reflection.cpp
)

target_link_libraries(tinyrefl-tool-tests PRIVATE tinyrefl-tool-lib)
target_include_directories(tinyrefl-tool-tests PRIVATE "${TINYREFL_SOURCE_DIR}/tests")

tinyrefl_tool(TARGET tinyrefl-tool-tests HEADERS
    dummy_folder/dummy_header.hpp
)
