include(FetchContent)
include(${TINYREFL_SOURCE_DIR}/cmake/utils.cmake)

function(require_target NAME)
    if(NOT TARGET ${NAME})
        message(FATAL_ERROR "\"${NAME}\" is not a target")
    else()
        message(STATUS "Target \"${NAME}\" found")
    endif()
endfunction()

function(require_targets)
    foreach(name ${ARGN})
        require_target(${name})
    endforeach()
endfunction()

find_package(Git REQUIRED)

macro(external_dependency NAME URL COMMIT)
    string(TOLOWER "${NAME}" name)
    include(CMakeParseArguments)
    cmake_parse_arguments(EXTERNAL
        ""
        "SOURCE_SUBDIR"
        ""
        ${ARGN})

    if(EXTERNAL_SOURCE_SUBDIR)
        message(STATUS "Configuring external dependency ${NAME} using subdir ${EXTERNAL_SOURCE_SUBDIR}")
        set(sourceSubdir "SOURCE_SUBDIR ${EXTERNAL_SOURCE_SUBDIR}")
    else()
        set(sourceSubdir)
    endif()

    set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

    FetchContent_Declare(${NAME}
        GIT_REPOSITORY "${URL}"
        GIT_TAG "${COMMIT}"
    )

    if(TARGET ${NAME})
        message(STATUS "Skipping external dependency ${NAME}")
    else()
        message(STATUS "external dependency ${NAME} from ${URL} at ${COMMIT}")

        FetchContent_GetProperties(${name})

        if(EXTERNAL_SOURCE_SUBDIR)
            set(sourceSubdir "/${EXTERNAL_SOURCE_SUBDIR}")
        else()
            set(sourceSubdir)
        endif()

        if(NOT ${name}_POPULATED)
            FetchContent_Populate(${NAME})
            message(STATUS "external dependency ${NAME} populated")
            add_subdirectory("${${name}_SOURCE_DIR}${sourceSubdir}" "${${name}_BINARY_DIR}" EXCLUDE_FROM_ALL)
        endif()

        set(${NAME}_SOURCE_DIR "${${NAME}_SOURCE_DIR}" CACHE PATH "")
        set(${NAME}_BINARY_DIR "${${NAME}_BINARY_DIR}" CACHE PATH "")

        if(TARGET ${NAME})
            mark_as_external_target(${NAME})
        endif()
    endif()
endmacro()
