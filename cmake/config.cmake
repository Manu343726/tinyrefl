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

    if(HAS_NOEXCEPT_ABI_WARNING)
        set(no_noexcept_abi_warning "-Wno-noexcept-type")
    endif()

    set(TINYREFL_CXX_WARNINGS -Wall -Werror -Wno-attributes ${no_noexcept_abi_warning})
endif()
