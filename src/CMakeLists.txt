add_library(tinyrefl utils/demangle.cpp)

target_link_libraries(tinyrefl PUBLIC fmt ctti masquerade)
target_include_directories(tinyrefl PUBLIC "${TINYREFL_INCLUDE_DIR}")
