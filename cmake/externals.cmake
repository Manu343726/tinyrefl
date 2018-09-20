include(${TINYREFL_SOURCE_DIR}/cmake/DownloadProject.cmake)

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

macro(external_dependency NAME URL COMMIT)
    message(STATUS "external dependency ${NAME} from ${URL} at ${COMMIT}")
    download_project(
        PROJ "${NAME}"
        GIT_REPOSITORY "${URL}"
        GIT_TAG "${COMMIT}"
        UPDATE_DISCONNECTED 1
        GIT_SHALLOW 1
    )
    add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
    set(${NAME}_SOURCE_DIR "${${NAME}_SOURCE_DIR}" CACHE PATH "")
    set(${NAME}_BINARY_DIR "${${NAME}_BINARY_DIR}" CACHE PATH "")
endmacro()
