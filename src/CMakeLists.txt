add_library(tinyrefl utils/demangle.cpp)

target_link_libraries(tinyrefl PUBLIC fmt ctti nlohmann_json)
target_include_directories(tinyrefl PUBLIC "${TINYREFL_INCLUDE_DIR}")
