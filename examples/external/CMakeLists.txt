include(${TINYREFL_SOURCE_DIR}/cmake/externals.cmake)
find_package(Boost)

set(BUILD_EXAMPLES OFF CACHE BOOL "disable rttr examples")
set(BUILD_UNIT_TESTS OFF CACHE BOOL "disable rttr unit tests")
set(BUILD_DOCUMENTATION OFF CACHE BOOL "disable rttr docs")
set(BUILD_INSTALLER OFF CACHE BOOL "disable rttr installer")
set(BUILD_PACKAGE OFF CACHE BOOL "disable rttr CPack")

external_dependency(rttr https://github.com/rttrorg/rttr.git v0.9.6)
