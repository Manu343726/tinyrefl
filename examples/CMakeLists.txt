include(external/external.cmake)
include(${TINYREFL_SOURCE_DIR}/cmake/CheckCXXConcepts.cmake)

add_library(tinyrefl-example-lib example.cpp example2.cpp)
target_compile_options(tinyrefl-example-lib PRIVATE ${TINYREFL_CXX_WARNINGS})
tinyrefl_tool(TARGET tinyrefl-example-lib HEADERS example.hpp example2.hpp)

add_tinyrefl_example(tinyrefl-example api.cpp)
target_compile_options(tinyrefl-example PRIVATE ${TINYREFL_CXX_WARNINGS})

add_tinyrefl_example(hello-world hello_world.cpp)
set_target_properties(hello-world PROPERTIES CXX_STANDARD 17)
target_compile_options(hello-world PRIVATE ${TINYREFL_CXX_WARNINGS})

set(concepts_feature_flags -fconcepts)
set(template_max_depth_flag -ftemplate-depth)

set(CMAKE_REQUIRED_FLAGS ${concepts_feature_flags})
check_cxx_concepts(CXX_COMPILER_HAS_CONCEPTS)

option(TINYREFL_EXAMPLES_CONCEPTS "Enable Concepts TS support for examples" OFF)

if(TINYREFL_EXAMPLES_CONCEPTS AND CXX_COMPILER_HAS_CONCEPTS)
    target_compile_options(tinyrefl-example PRIVATE ${concepts_feature_flags})
    message(STATUS "tinyrefl-example compiled with Concepts TS support")
else()
    message(STATUS "tinyrefl-example compiled witouth Concepts TS support")
endif()

if(NOT MSVC)
    if(NOT TINYREFL_MAX_TEMPLATE_DEPTH)
        set(TINYREFL_MAX_TEMPLATE_DEPTH 10000)
    endif()

    message(STATUS "Example configured with ${TINYREFL_MAX_TEMPLATE_DEPTH} max template instantiation depth")
    target_compile_options(tinyrefl-example PRIVATE -ftemplate-depth=${TINYREFL_MAX_TEMPLATE_DEPTH})
endif()

target_link_libraries(tinyrefl-example PRIVATE tinyrefl-example-lib)
target_link_libraries(hello-world PRIVATE tinyrefl-example-lib)

if(TINYREFL_BACKEND_EXAMPLES_UPDATED_TO_V05X_API)
    if(Boost_FOUND)
        if((CMAKE_CXX_COMPILER_ID STREQUAL GNU) AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0.0))
            message(WARNING "Versions of GCC prior to 6.0.0 are not supported by Boost.Hana. The example will not be built")
        else()
            add_tinyrefl_example(tinyrefl-boost-hana-example boost_hana.cpp)
            target_compile_options(tinyrefl-boost-hana-example PRIVATE ${TINYREFL_CXX_WARNINGS})
            target_link_libraries(tinyrefl-boost-hana-example PRIVATE tinyrefl-example-lib)
            target_include_directories(tinyrefl-boost-hana-example PRIVATE ${Boost_INCLUDE_DIRS})
        endif()

        add_tinyrefl_example(tinyrefl-boost-fusion-example boost_fusion.cpp)
        target_compile_options(tinyrefl-boost-fusion-example PRIVATE ${TINYREFL_CXX_WARNINGS})
        target_link_libraries(tinyrefl-boost-fusion-example PRIVATE tinyrefl-example-lib)
        target_include_directories(tinyrefl-boost-fusion-example PRIVATE ${Boost_INCLUDE_DIRS})
    else()
        message(WARNING "Boost not found, boost example will not be built")
    endif()

    add_tinyrefl_example(tinyrefl-rttr-example rttr.cpp)
    target_compile_options(tinyrefl-rttr-example PRIVATE ${TINYREFL_CXX_WARNINGS})
    target_link_libraries(tinyrefl-rttr-example PRIVATE tinyrefl-example-lib RTTR::Core)

    add_tinyrefl_example(tinyrefl-metastuff-example metastuff.cpp)
    target_compile_options(tinyrefl-metastuff-example PRIVATE ${TINYREFL_CXX_WARNINGS})
    target_link_libraries(tinyrefl-metastuff-example PRIVATE tinyrefl-example-lib MetaStuff)
else()
    message(WARNING "TODO: Update backend examples to v5.0.x API")
endif()
