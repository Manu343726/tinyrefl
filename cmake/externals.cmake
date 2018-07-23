include(${TINYREFL_SOURCE_DIR}/cmake/DownloadProject.cmake)

macro(external_dependency NAME URL COMMIT)
    if(NOT TARGET ${NAME})
        message(STATUS "external dependency ${NAME} from ${URL} at ${COMMIT}")
        download_project(
            PROJ "${NAME}"
            GIT_REPOSITORY "${URL}"
            GIT_TAG "${COMMIT}"
        )
        add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})

        if(NOT TARGET ${NAME})
            message(FATAL_ERROR "Target ${NAME} not found after download")
        endif()
    else()
        message(STATUS "external dependency ${NAME} already satisfied")
    endif()
endmacro()
