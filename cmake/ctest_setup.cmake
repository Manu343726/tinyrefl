
function(add_tinyrefl_test NAME)
    add_executable(${NAME} ${ARGN})

    find_program(MEMORYCHECK_COMMAND valgrind)

    if(NOT TINYREFL_VALGRIND_MAX_BACKTRACE_DEPTH)
        set(TINYREFL_VALGRIND_MAX_BACKTRACE_DEPTH 100)
    endif()

    if(MEMORYCHECK_COMMAND)
        message(STATUS "Running CTest ${NAME} with valgrind")
        add_test(NAME ${NAME} COMMAND ${MEMORYCHECK_COMMAND} --leak-check=full --num-callers=${TINYREFL_VALGRIND_MAX_BACKTRACE_DEPTH} $<TARGET_FILE:${NAME}>)
    else()
        add_test(${NAME} ${NAME})
    endif()
endfunction()

function(add_tinyrefl_example NAME)
    add_tinyrefl_test(${NAME} ${ARGN})
endfunction()

enable_testing()
