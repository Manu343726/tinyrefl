add_tinyrefl_test(tinyrefl-tool-tests
    main.cpp
    driver_handles_headers_in_different_dirs.cpp
    jinja2reflection.cpp
    cppast_jinja2reflection.cpp
    templates/macro_based_codegen/model.cpp
)

target_link_libraries(tinyrefl-tool-tests PRIVATE tinyrefl-tool-lib)
target_include_directories(tinyrefl-tool-tests PRIVATE "${TINYREFL_SOURCE_DIR}/tests")
add_variable_compile_definition(tinyrefl-tool-tests TINYREFL_TOOL_SOURCE_DIR STRING)
add_variable_compile_definition(tinyrefl-tool-tests TINYREFL_INCLUDE_DIR STRING)

tinyrefl_tool(TARGET tinyrefl-tool-tests HEADERS
    dummy_folder/dummy_header.hpp
)
