include(${TINYREFL_SOURCE_DIR}/cmake/externals.cmake)
include(${TINYREFL_SOURCE_DIR}/cmake/utils.cmake)

find_package(Boost)

if(Boost_FOUND)
    message(STATUS "Found Boost version ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
endif()

set(BUILD_EXAMPLES OFF CACHE BOOL "disable rttr examples")
set(BUILD_UNIT_TESTS OFF CACHE BOOL "disable rttr unit tests")
set(BUILD_DOCUMENTATION OFF CACHE BOOL "disable rttr docs")
set(BUILD_INSTALLER OFF CACHE BOOL "disable rttr installer")
set(BUILD_PACKAGE OFF CACHE BOOL "disable rttr CPack")

external_dependency(fmt https://github.com/fmtlib/fmt.git 5.3.0)
external_dependency(rttr https://github.com/rttrorg/rttr.git v0.9.6)
external_dependency(MetaStuff https://github.com/Manu343726/MetaStuff master)
mark_as_external_target(rttr_core)
mark_as_external_target(MetaStuff)
