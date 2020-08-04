add_library(tinyrefl utils/demangle.cpp)

if(NOT MSVC)
    target_compile_options(tinyrefl INTERFACE -Wno-gnu-string-literal-operator-template)
endif()

target_compile_options(tinyrefl PRIVATE ${TINYREFL_COMPILE_OPTIONS})

target_link_libraries(tinyrefl PUBLIC tinyrefl_externals_ctti tinyrefl_externals_jsonformoderncpp)
target_include_directories(tinyrefl PUBLIC "${TINYREFL_INCLUDE_DIR}")
define_tinyrefl_version_variables(tinyrefl)

try_compile(TINYREFL_HAS_CONSTEXPR_ARRAY_VIEW_SUBSCRIPT
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/check_constexpr_array_view_subscript.cpp"
    CMAKE_FLAGS -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
    OUTPUT_VARIABLE output)

if(TINYREFL_HAS_CONSTEXPR_ARRAY_VIEW_SUBSCRIPT)
    target_compile_definitions(tinyrefl PUBLIC TINYREFL_HAS_CONSTEXPR_ARRAY_VIEW_SUBSCRIPT)
else()
    message(WARNING "Constexpr array_view subscript operator not supported by the compiler. Build error: ${output}")
ENDIF()
