add_library(tinyrefl-tool-model
    enum.cpp
)

target_include_directories(tinyrefl-tool-model PUBLIC "${TINYREFL_TOOL_INCLUDE_DIR}")
target_link_libraries(tinyrefl-tool-model PUBLIC tinyrefl)
