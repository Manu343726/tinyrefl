add_subdirectory(detail)
add_subdirectory(model)

add_library(tinyrefl-tool-lib
    argument_parser.cpp)

add_variable_compile_definition(tinyrefl-tool-lib TINYREFL_TOOL_TEMPLATES_DIR STRING)
add_variable_compile_definition(tinyrefl-tool-lib TINYREFL_TOOL_DEFAULT_TEMPLATE STRING)
define_tinyrefl_version_variables(tinyrefl-tool-lib)
define_llvm_version_variables(tinyrefl-tool-lib)

target_include_directories(tinyrefl-tool-lib PUBLIC "${TINYREFL_TOOL_INCLUDE_DIR}")
target_link_libraries(tinyrefl-tool-lib PUBLIC tinyrefl-tool-model tinyrefl-tool-detail)

find_package(LLVM ${TINYREFL_LLVM_VERSION} REQUIRED EXACT CONFIG PATHS "${LLVM_CMAKE_PATH}" NO_DEFAULT_PATH)
target_link_libraries(tinyrefl-tool-lib PUBLIC cppast cppfs string-view-lite LLVMSupport)
