include(${TINYREFL_SOURCE_DIR}/utils.cmake)

function(tinyrefl_tool)
    cmake_parse_arguments(
        ARGS
        ""
        "TARGET"
        "HEADERS"
        ${ARGN}
    )

    if(NOT ARGS_TARGET)
        message(FATAL_ERROR "No TARGET given to tinyrefl tool driver")
    endif()
    if(NOT ARGS_HEADERS)
        message(FATAL_ERROR "No HEADERS given to tinyrefl tool driver")
    endif()

    target_link_libraries(${ARGS_TARGET} PUBLIC tinyrefl)

    get_target_include_directories(${ARGS_TARGET} includes)
    clangxx_stdlib_includes(stdlibc++ stdlib_includes)

    set(compile_options ${options} ${include_options})

    foreach(header ${ARGS_HEADERS})
        add_prebuild_command(TARGET ${ARGS_TARGET}
            NAME "tinyrefl_tool_${ARGS_TARGET}_${header}"
            COMMAND tinyrefl-tool ${header} ${CMAKE_CXX_STANDARD} ${includes} ${stdlib_includes}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating tinyrefl metadata for ${ARGS_TARGET}/${header}"
        )
    endforeach()

    add_dependencies(${ARGS_TARGET} tinyrefl-tool)

    string(REGEX REPLACE ";" " " header_list "${ARGS_HEADERS}")
    string(REGEX REPLACE ";" " " options_list "${compile_options}")
    message(STATUS ">> Tinyrefl driver on ${ARGS_TARGET}: tinyrefl ${header_list} -std=c++${CMAKE_CXX_STANDARD} ${includes}")
endfunction()

