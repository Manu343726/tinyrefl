
find_package(Git REQUIRED)

execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive)

add_subdirectory(cppast)
add_subdirectory(ctti)
add_subdirectory(fmt)
add_subdirectory(masquerade)
