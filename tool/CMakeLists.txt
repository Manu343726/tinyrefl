
if(TINYREFL_CROSS_BUILDING)
    message(STATUS "Cross-building tinyrefl, host-build tinyrefl-tool")
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
            -DTINYREFL_LLVM_DOWNLOAD_FROM_OFFICIAL_SERVER=${TINYREFL_LLVM_DOWNLOAD_FROM_OFFICIAL_SERVER}
            -DTINYREFL_LLVM_DOWNLOAD_URL=${TINYREFL_LLVM_DOWNLOAD_URL}
            -DTINYREFL_CPPAST_REPO_URL=${TINYREFL_CPPAST_REPO_URL}
            -DTINYREFL_CPPAST_VERSION=${TINYREFL_CPPAST_VERSION}
            -DTINYREFL_FMT_REPO_URL=${TINYREFL_FMT_REPO_URL}
            -DTINYREFL_FMT_VERSION=${TINYREFL_FMT_VERSION}
            -DTINYREFL_CPPFS_REPO_URL=${TINYREFL_CPPFS_REPO_URL}
            -DTINYREFL_CPPFS_VERSION=${TINYREFL_CPPFS_VERSION}
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
    require_targets(cppast cppfs)

    # LLVM releases are compiled with old GCC ABI and RTTI disabled
    if(NOT TINYREFL_USE_LOCAL_LLVM)
        if(NOT MSVC)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 -fno-rtti")
        endif()
    endif()

    add_executable(tinyrefl-tool tool.cpp)
    target_include_directories(tinyrefl-tool PRIVATE ${TINYREFL_INCLUDE_DIR} ${fmt-header-only_SOURCE_DIR}/include)
    target_compile_definitions(tinyrefl-tool PRIVATE -DFMT_HEADER_ONLY)

    find_package(LLVM ${TINYREFL_LLVM_VERSION} REQUIRED EXACT CONFIG PATHS "${LLVM_CMAKE_PATH}" NO_DEFAULT_PATH)
    target_link_libraries(tinyrefl-tool PRIVATE cppast cppfs LLVMSupport)

    set(TINYREFL_TOOL_EXECUTABLE $<TARGET_FILE:tinyrefl-tool> CACHE STRING "")
    set(TINYREFL_TOOL_TARGET tinyrefl-tool CACHE STRING "")
endif()
