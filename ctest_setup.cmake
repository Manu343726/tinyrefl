
function(add_tinyrefl_test NAME)
    add_executable(${NAME} ${ARGN})

    find_program(MEMORYCHECK_COMMAND valgrind)

    if(MEMORYCHECK_COMMAND)
        message(STATUS "Running CTest ${NAME} with valgrind")
        add_test(NAME ${NAME} COMMAND ${MEMORYCHECK_COMMAND} --leak-check=full $<TARGET_FILE:${NAME}>)
    else()
        add_test(${NAME} ${NAME})
    endif()
endfunction()

enable_testing()
