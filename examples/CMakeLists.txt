include(${TINYREFL_SOURCE_DIR}/tool/driver.cmake)

add_library(tinyrefl-example-lib example.cpp)
tinyrefl_tool(TARGET tinyrefl-example-lib HEADERS example.hpp)

add_executable(tinyrefl-example api.cpp)

target_link_libraries(tinyrefl-example PRIVATE tinyrefl-example-lib)

find_package(Boost)

if(Boost_FOUND)
    add_executable(tinyrefl-boost-example boost.cpp)
    target_link_libraries(tinyrefl-boost-example PRIVATE tinyrefl-example-lib)
    target_include_directories(tinyrefl-boost-example PRIVATE ${Boost_INCLUDE_DIRS})
else()
    message(WARNING "Boost not found, boost example will not be build")
endif()
