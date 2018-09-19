include(${TINYREFL_SOURCE_DIR}/cmake/git.cmake)
include(${TINYREFL_SOURCE_DIR}/cmake/utils.cmake)

parse_version_number(${TINYREFL_LLVM_VERSION} TINYREFL_LLVM_VERSION_MAJOR TINYREFL_LLVM_VERSION_MINOR TINYREFL_LLVM_VERSION_FIX)

if(TINYREFL_CROSS_BUILDING)
    message(STATUS "Cross-building tinyrefl, host-build tinyrefl-tool")
    include(ExternalProject)

    set(srcdir ${CMAKE_CURRENT_LIST_DIR})
    set(bindir ${CMAKE_CURRENT_BINARY_DIR}/tinyrefl_tool_external)

    if(TINYREFL_HOST_CXX_COMPILER)
        set(cmake_cxx_compiler "-DCMAKE_CXX_COMPILER=\"${TINYREFL_HOST_CXX_COMPILER}\"")
    endif()

    if(TINYREFL_HOST_C_COMPILER)
        set(cmake_c_compiler "-DCMAKE_C_COMPILER=\"${TINYREFL_HOST_C_COMPILER}\"")
    endif()

    if(TINYREFL_HOST_CMAKE_GENERATOR)
        set(cmake_generator "-G \"${TINYREFL_HOST_CMAKE_GENERATOR}\"")
    endif()

    ExternalProject_Add(tinyrefl_tool_external
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${srcdir}
        BINARY_DIR ${bindir}
        CMAKE_ARGS
            ${cmake_generator}
            ${cmake_cxx_compiler}
            ${cmake_c_compiler}
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

    set(TINYREFL_TOOL_INCLUDE_DIR "${TINYREFL_SOURCE_DIR}/tool/include")

    add_subdirectory(external)
    add_subdirectory(src)
    require_targets(cppast cppfs)

    # LLVM releases are compiled with old GCC ABI and RTTI disabled
    if(NOT TINYREFL_USE_LOCAL_LLVM)
        if(NOT MSVC)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 -fno-rtti")
        endif()
    endif()

    add_executable(tinyrefl-tool tool.cpp)
    define_tinyrefl_version_variables(tinyrefl-tool)
    define_llvm_version_variables(tinyrefl-tool)
    target_include_directories(tinyrefl-tool PRIVATE ${fmt-header-only_SOURCE_DIR}/include)
    target_compile_definitions(tinyrefl-tool PRIVATE -DFMT_HEADER_ONLY)

    find_package(LLVM ${TINYREFL_LLVM_VERSION} REQUIRED EXACT CONFIG PATHS "${LLVM_CMAKE_PATH}" NO_DEFAULT_PATH)
    target_link_libraries(tinyrefl-tool PRIVATE tinyrefl-tool-model cppast cppfs LLVMSupport jinja2cpp)

    set(TINYREFL_TOOL_EXECUTABLE $<TARGET_FILE:tinyrefl-tool> CACHE STRING "")
    set(TINYREFL_TOOL_TARGET tinyrefl-tool CACHE STRING "")
endif()
