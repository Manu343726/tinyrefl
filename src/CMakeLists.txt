add_library(tinyrefl
    tinyrefl.cpp
)

target_link_libraries(tinyrefl PUBLIC cppast fmt ctti)
target_include_directories(tinyrefl PUBLIC "${CMAKE_SOURCE_DIR}/include")

add_library(tinyrefl_backend INTERFACE)
target_include_directories(tinyrefl_backend INTERFACE "${CMAKE_SOURCE_DIR}/include")
target_compile_definitions(tinyrefl_backend INTERFACE
    "-DTINYREFL_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}/include\""
    "-DCTTI_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}/ctti/include/\""
    "-DFMT_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}/fmt/\""
)

target_link_libraries(tinyrefl PUBLIC tinyrefl_backend)
