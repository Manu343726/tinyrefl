include(${TINYREFL_SOURCE_DIR}/cmake/DownloadProject.cmake)
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
    if(TARGET ${NAME})
        message(STATUS "Skipping external dependency ${NAME}")
    else()
        message(STATUS "external dependency ${NAME} from ${URL} at ${COMMIT}")
        download_project(
            PROJ "${NAME}"
            DOWNLOAD_COMMAND ${GIT_EXECUTABLE} clone ${URL} --depth=1 --branch=${COMMIT} -- ${CMAKE_CURRENT_BINARY_DIR}/${NAME}-src
            UPDATE_DISCONNECTED 1
            PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
            INSTALL ""
        )
        add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR} EXCLUDE_FROM_ALL)
        set(${NAME}_SOURCE_DIR "${${NAME}_SOURCE_DIR}" CACHE PATH "")
        set(${NAME}_BINARY_DIR "${${NAME}_BINARY_DIR}" CACHE PATH "")

        if(TARGET ${NAME})
            mark_as_external_target(${NAME})
        endif()
    endif()
endmacro()
