add_library(tinyrefl-tool-detail
    logger.cpp
)

target_include_directories(tinyrefl-tool-detail PUBLIC "${TINYREFL_TOOL_INCLUDE_DIR}")
target_link_libraries(tinyrefl-tool-detail PUBLIC debug_assert spdlog)
