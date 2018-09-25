add_library(tinyrefl-tool-model
    cppast/common.cpp
    cppast/class.cpp
    cppast/jinja2reflection.cpp
    enum.cpp
)

target_include_directories(tinyrefl-tool-model PUBLIC "${TINYREFL_TOOL_INCLUDE_DIR}")
target_link_libraries(tinyrefl-tool-model PUBLIC tinyrefl cppast jinja2cpp fmt)
