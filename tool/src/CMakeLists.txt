add_library(tinyrefl-tool-lib
    visitor.cpp
    entity_names.cpp
    serializer.cpp
    string_registry.cpp
    entity_registry.cpp
    serializers/value_serializer.cpp
    serializers/type_serializer.cpp
    serializers/sequence_serializer.cpp
    serializers/entity_serializer.cpp
    serializers/registration_serializer.cpp
)

target_include_directories(tinyrefl-tool-lib
    PUBLIC "${TINYREFL_TOOL_INCLUDE_DIR}"
    PUBLIC "${TINYREFL_INCLUDE_DIR}")

target_link_libraries(tinyrefl-tool-lib PUBLIC
    tinyrefl_externals_cppast
    tinyrefl_externals_fmt
    tinyrefl_externals_type_safe
    tinyrefl_externals_cppfs
    tinyrefl_externals_spdlog
    tinyrefl_externals_ctti
    tinyrefl_externals_jsonformoderncpp
)
