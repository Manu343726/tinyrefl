include(${TINYREFL_SOURCE_DIR}/tool/driver.cmake)

add_library(tinyrefl-test-static
    backend.cpp
    generated_code.cpp
    api.cpp
    invokable_traits.cpp
)

target_link_libraries(tinyrefl-test-static PRIVATE tinyrefl)

tinyrefl_tool(TARGET tinyrefl-test-static
HEADERS
    strings.hpp
    members.hpp
)
