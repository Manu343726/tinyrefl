
find_package(Git REQUIRED)

execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive)

set(CPPAST_BUILD_EXAMPLE OFF CACHE BOOL "disable cppast examples")
set(CPPAST_BUILD_TEST OFF CACHE BOOL "disable cppast tests")
set(CPPAST_BUILD_TOOL OFF CACHE BOOL "disable cppast tool")
set(LLVM_PREFERRED_VERSION 3.9.1)
set(LLVM_DOWNLOAD_OS_NAME "x86_64-linux-gnu-ubuntu-14.04")

add_subdirectory(cppast)
add_subdirectory(ctti)
add_subdirectory(fmt)
add_subdirectory(masquerade)
