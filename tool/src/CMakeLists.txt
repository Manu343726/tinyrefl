protobuf_generate_cpp(proto_srcs proto_hdrs model/ast.proto)

file(READ model/ast.proto AST_PROTO)
configure_file(model/schema.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/schema.cpp @ONLY)

add_library(
    tinyrefl-tool-lib
    backends/legacy/entity_names.cpp
    backends/legacy/serializer.cpp
    backends/legacy/string_registry.cpp
    backends/legacy/entity_registry.cpp
    backends/legacy/attribute_registry.cpp
    backends/legacy/serializers/value_serializer.cpp
    backends/legacy/serializers/type_serializer.cpp
    backends/legacy/serializers/sequence_serializer.cpp
    backends/legacy/serializers/entity_serializer.cpp
    backends/legacy/serializers/registration_serializer.cpp
    backends/cppast/identifier.cpp
    backends/cppast/importer.cpp
    backends/cppast/attribute.cpp
    backends/cppast/datareader.cpp
    backends/cppast/astreporter.cpp
    backends/cppast/hierarchy.cpp
    backends/cppast/source_location.cpp
    backends/cppast/parserdiagnosticslogger.cpp
    model/importer.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/schema.cpp
    exporters/json.cpp
    exporters/generic.cpp
    exporters/protobuf_text.cpp
    exporters/basic_exporter.cpp
    exporters/protobuf_binary.cpp
    exporters/protobuf_base64_binary.cpp
    exporters/detail/base64/base64.cpp
    ${proto_srcs}
    diagnosticreporter.cpp
    unique_id.cpp)

target_compile_options(tinyrefl-tool-lib PUBLIC ${TINYREFL_CXX_WARNINGS})

target_include_directories(
    tinyrefl-tool-lib
    PUBLIC "${TINYREFL_TOOL_INCLUDE_DIR}"
    PUBLIC "${TINYREFL_INCLUDE_DIR}"
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(
    tinyrefl-tool-lib
    PUBLIC tinyrefl_externals_cppast
           tinyrefl_externals_fmt
           tinyrefl_externals_type_safe
           tinyrefl_externals_cppfs
           tinyrefl_externals_spdlog
           tinyrefl_externals_ctti
           tinyrefl_externals_jsonformoderncpp
           tinyrefl_externals_protobuf)

if(NOT MSVC)
    target_compile_options(tinyrefl-tool-lib
                           PUBLIC -Wno-gnu-string-literal-operator-template)
endif()
