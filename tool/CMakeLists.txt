add_executable(tinyrefl-tool tool.cpp)

target_compile_definitions(tinyrefl-tool PRIVATE
    "-DTINYREFL_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}/include\""
    "-DCTTI_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}/ctti/include/\""
    "-DFMT_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}/fmt/\""
    "-DMASQUERADE_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}/masquerade\""
)

target_include_directories(tinyrefl-tool PRIVATE ${TINYREFL_INCLUDE_DIR})
target_link_libraries(tinyrefl-tool PRIVATE cppast fmt masquerade)
