include(external/external.cmake)
include(${TINYREFL_SOURCE_DIR}/cmake/CheckCXXConcepts.cmake)

add_library(tinyrefl-example-lib example.cpp example2.cpp)
tinyrefl_tool(TARGET tinyrefl-example-lib HEADERS example.hpp example2.hpp)

add_tinyrefl_example(tinyrefl-example api.cpp)

set(concepts_feature_flags -fconcepts)

set(CMAKE_REQUIRED_FLAGS ${concepts_feature_flags})
check_cxx_concepts(CXX_COMPILER_HAS_CONCEPTS)

if(TRUE)
    target_compile_options(tinyrefl-example PRIVATE ${concepts_feature_flags})
    message(STATUS "tinyrefl-example compiled with Concepts TS support")
else()
    message(STATUS "tinyrefl-example compiled witouth Concepts TS support")
endif()

target_link_libraries(tinyrefl-example PRIVATE tinyrefl-example-lib)

if(TINYREFL_BACKEND_EXAMPLES_UPDATED_TO_V05X_API)
    if(Boost_FOUND)
        if((CMAKE_CXX_COMPILER_ID STREQUAL GNU) AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0.0))
            message(WARNING "Versions of GCC prior to 6.0.0 are not supported by Boost.Hana. The example will not be built")
        else()
            add_tinyrefl_example(tinyrefl-boost-hana-example boost_hana.cpp)
            target_link_libraries(tinyrefl-boost-hana-example PRIVATE tinyrefl-example-lib)
            target_include_directories(tinyrefl-boost-hana-example PRIVATE ${Boost_INCLUDE_DIRS})
        endif()

        add_tinyrefl_example(tinyrefl-boost-fusion-example boost_fusion.cpp)
        target_link_libraries(tinyrefl-boost-fusion-example PRIVATE tinyrefl-example-lib)
        target_include_directories(tinyrefl-boost-fusion-example PRIVATE ${Boost_INCLUDE_DIRS})
    else()
        message(WARNING "Boost not found, boost example will not be built")
    endif()

    add_tinyrefl_example(tinyrefl-rttr-example rttr.cpp)
    target_link_libraries(tinyrefl-rttr-example PRIVATE tinyrefl-example-lib RTTR::Core)

    add_tinyrefl_example(tinyrefl-metastuff-example metastuff.cpp)
    target_link_libraries(tinyrefl-metastuff-example PRIVATE tinyrefl-example-lib MetaStuff)
else()
    message(WARNING "TODO: Update backend examples to v5.0.x API")
endif()
