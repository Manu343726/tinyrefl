if(TINYREFL_CROSS_BUILDING)
    include(ExternalProject)

    set(srcdir ${CMAKE_CURRENT_LIST_DIR})
    set(bindir ${CMAKE_CURRENT_BINARY_DIR}/tinyrefl_tool_external)

    ExternalProject_Add(tinyrefl_tool_external
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${srcdir}
        BINARY_DIR ${bindir}
        CMAKE_ARGS
            -DTINYREFL_HOST_BUILDING_TOOL=ON
            -DTINYREFL_SOURCE_DIR=${TINYREFL_SOURCE_DIR}
            -DTINYREFL_BINARY_DIR=${TINYREFL_BINARY_DIR}
            -DTINYREFL_INCLUDE_DIR=${TINYREFL_INCLUDE_DIR}
            -DTINYREFL_LLVM_VERSION=${TINYREFL_LLVM_VERSION}
            -DTINYREFL_USE_LOCAL_LLVM=${TINYREFL_USE_LOCAL_LLVM}
        INSTALL_COMMAND ""
    )

    set(TINYREFL_TOOL_EXECUTABLE ${bindir}/tinyrefl-tool CACHE STRING "")
    set(TINYREFL_TOOL_TARGET tinyrefl_tool_external CACHE STRING "")
else()
    if(TINYREFL_HOST_BUILDING_TOOL)
        cmake_minimum_required(VERSION 3.0)
        project(tinyrefl-tool)
        set(CMAKE_CXX_STANDARD 14)
    endif()

    add_subdirectory(external)
    add_executable(tinyrefl-tool tool.cpp)

    target_compile_definitions(tinyrefl-tool PRIVATE
        "-DTINYREFL_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}/include\""
        "-DCTTI_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}/ctti/include/\""
        "-DFMT_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}/fmt/\""
        "-DMASQUERADE_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}/masquerade\""
    )

    target_include_directories(tinyrefl-tool PRIVATE ${TINYREFL_INCLUDE_DIR})
    target_link_libraries(tinyrefl-tool PRIVATE cppast fmt masquerade cppfs llvm-commandline)

    set(TINYREFL_TOOL_EXECUTABLE $<TARGET_FILE:tinyrefl-tool> CACHE STRING "")
    set(TINYREFL_TOOL_TARGET tinyrefl-tool CACHE STRING "")
endif()
