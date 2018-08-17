add_subdirectory(external)

add_library(tinyrefl-example-lib example.cpp)
tinyrefl_tool(TARGET tinyrefl-example-lib HEADERS example.hpp)

add_tinyrefl_example(tinyrefl-example api.cpp)

target_link_libraries(tinyrefl-example PRIVATE tinyrefl-example-lib)

if(Boost_FOUND)
    add_tinyrefl_example(tinyrefl-boost-hana-example boost_hana.cpp)
    add_tinyrefl_example(tinyrefl-boost-fusion-example boost_fusion.cpp)
    target_link_libraries(tinyrefl-boost-hana-example PRIVATE tinyrefl-example-lib)
    target_link_libraries(tinyrefl-boost-fusion-example PRIVATE tinyrefl-example-lib)
    target_include_directories(tinyrefl-boost-hana-example PRIVATE ${Boost_INCLUDE_DIRS})
    target_include_directories(tinyrefl-boost-fusion-example PRIVATE ${Boost_INCLUDE_DIRS})
else()
    message(WARNING "Boost not found, boost example will not be build")
endif()

add_tinyrefl_example(tinyrefl-rttr-example rttr.cpp)
target_link_libraries(tinyrefl-rttr-example PUBLIC tinyrefl-example-lib RTTR::Core)
