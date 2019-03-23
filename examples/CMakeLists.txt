include(external/external.cmake)

add_library(tinyrefl-example-lib example.cpp)
tinyrefl_tool(TARGET tinyrefl-example-lib HEADERS example.hpp example2.hpp)

add_tinyrefl_example(tinyrefl-example api.cpp)

target_link_libraries(tinyrefl-example PRIVATE tinyrefl-example-lib)

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
