add_library(tinyrefl-tool-model
    cppast/common.cpp
    cppast/class.cpp
    cppast/enum.cpp
    cppast/file.cpp
    cppast/attribute.cpp
    cppast/token.cpp
    cppast/jinja2reflection.cpp
    enum.cpp
)

target_include_directories(tinyrefl-tool-model PUBLIC "${TINYREFL_TOOL_INCLUDE_DIR}")
target_link_libraries(tinyrefl-tool-model PUBLIC tinyrefl tinyrefl-tool-detail cppast jinja2cpp fmt)
