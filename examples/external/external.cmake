include(${TINYREFL_SOURCE_DIR}/cmake/externals.cmake)

find_package(Boost)

if(Boost_FOUND)
    message(STATUS "Found Boost version ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
endif()

set(BUILD_EXAMPLES OFF CACHE BOOL "disable rttr examples")
set(BUILD_UNIT_TESTS OFF CACHE BOOL "disable rttr unit tests")
set(BUILD_DOCUMENTATION OFF CACHE BOOL "disable rttr docs")
set(BUILD_INSTALLER OFF CACHE BOOL "disable rttr installer")
set(BUILD_PACKAGE OFF CACHE BOOL "disable rttr CPack")

external_dependency(rttr https://github.com/rttrorg/rttr.git v0.9.6)
