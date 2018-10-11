add_subdirectory(detail)
add_subdirectory(model)

add_library(tinyrefl-tool-lib ast_generator.cpp)
target_include_directories(tinyrefl-tool-lib PUBLIC "${TINYREFL_TOOL_INCLUDE_DIR}")
target_link_libraries(tinyrefl-tool-lib PUBLIC tinyrefl-tool-model tinyrefl-tool-detail)
