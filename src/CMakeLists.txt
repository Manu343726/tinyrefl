add_library(tinyrefl INTERFACE)

target_link_libraries(tinyrefl INTERFACE fmt ctti masquerade)
target_include_directories(tinyrefl INTERFACE "${TINYREFL_INCLUDE_DIR}")
