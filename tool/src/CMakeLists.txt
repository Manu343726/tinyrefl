add_subdirectory(model)

add_library(tinyrefl-tool-lib dummy.cpp)
target_include_directories(tinyrefl-tool-lib PUBLIC "${TINYREFL_TOOL_INCLUDE_DIR}")
target_link_libraries(tinyrefl-tool-lib PUBLIC tinyrefl-tool-model tinyrefl)
