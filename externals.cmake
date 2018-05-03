include(${TINYREFL_SOURCE_DIR}/DownloadProject.cmake)

macro(external_dependency NAME URL COMMIT)
    if(NOT TARGET ${NAME})
        message(STATUS "external dependency ${NAME} from ${URL} at ${COMMIT}")
        download_project(
            PROJ "${NAME}"
            GIT_REPOSITORY "${URL}"
            GIT_TAG "${COMMIT}"
        )
    add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
    else()
        message(STATUS "external dependency ${NAME} already satisfied")
    endif()
endmacro()
