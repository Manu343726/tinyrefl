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
    if(NOT TARGET ${NAME})
        message(STATUS "external dependency ${NAME} from ${URL} at ${COMMIT}")
        download_project(
            PROJ "${NAME}"
            GIT_REPOSITORY "${URL}"
            GIT_TAG "${COMMIT}"
        )
        add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
        require_target(${NAME})
    else()
        message(STATUS "external dependency ${NAME} already satisfied")
    endif()
endmacro()
