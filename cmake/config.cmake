include (CheckCXXCompilerFlag)

if(CMAKE_CXX_STANDARD)
    if(CMAKE_CXX_STANDARD LESS 14)
        message(FATAL_ERROR "tinyrefl requires C++14 at least, requested C++${CMAKE_CXX_STANDARD}")
    endif()
else()
    set(CMAKE_CXX_STANDARD 14)
endif()

message(STATUS "tinyrefl compiled with C++${CMAKE_CXX_STANDARD}")

if(NOT MSVC)
    check_cxx_compiler_flag("-Wnoexcept-type" HAS_NOEXCEPT_ABI_WARNING)
    check_cxx_compiler_flag("-Wno-unused-command-line-argument" HAS_WNO_UNUSED_COMMAND_LINE_ARGUMENT)

    if(HAS_NOEXCEPT_ABI_WARNING)
        set(no_noexcept_abi_warning "-Wno-noexcept-type")
    endif()

    if(HAS_WNO_UNUSED_COMMAND_LINE_ARGUMENT)
        set(no_unused_command_line_argument "-Wno-unused-command-line-argument")
    endif()

    set(TINYREFL_CXX_WARNINGS -Wall -Werror -Wno-attributes ${no_noexcept_abi_warning} ${no_unused_command_line_argument})
endif()
