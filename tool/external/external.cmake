include(${TINYREFL_SOURCE_DIR}/cmake/utils.cmake)
include(${TINYREFL_SOURCE_DIR}/cmake/externals.cmake)
include(${TINYREFL_SOURCE_DIR}/external/external.cmake)

set(CPPAST_BUILD_EXAMPLE
    OFF
    CACHE BOOL "disable cppast examples")
set(CPPAST_BUILD_TEST
    OFF
    CACHE BOOL "disable cppast tests")
set(CPPAST_BUILD_TOOL
    OFF
    CACHE BOOL "disable cppast tool")
set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "build cppfs as static lib")
set(OPTION_BUILD_TESTS
    OFF
    CACHE BOOL "disable cppfs tests")

if(NOT
   (
   "${TINYREFL_LLVM_VERSION}"
   STREQUAL
   "${TINYREFL_LLVM_VERSION_MAJOR}.${TINYREFL_LLVM_VERSION_MINOR}.${TINYREFL_LLVM_VERSION_FIX}"
   ))
    message(FATAL_ERROR "WTF????")
endif()

if(NOT TINYREFL_FMT_REPO_URL)
    set(TINYREFL_FMT_REPO_URL https://github.com/fmtlib/fmt.git)
endif()
if(NOT TINYREFL_FMT_VERSION)
    set(TINYREFL_FMT_VERSION 7.1.3)
endif()
if(NOT TINYREFL_CPPAST_REPO_URL)
    set(TINYREFL_CPPAST_REPO_URL https://github.com/Manu343726/cppast.git)
endif()
if(NOT TINYREFL_CPPAST_VERSION)
    set(TINYREFL_CPPAST_VERSION master)
endif()
if(NOT TINYREFL_CPPFS_REPO_URL)
    set(TINYREFL_CPPFS_REPO_URL https://github.com/Manu343726/cppfs.git)
endif()
if(NOT TINYREFL_CPPFS_VERSION)
    set(TINYREFL_CPPFS_VERSION optional_libSSL2)
endif()
if(NOT TINYREFL_TYPE_SAFE_URL)
    set(TINYREFL_TYPE_SAFE_URL "https://github.com/Manu343726/type_safe")
endif()
if(NOT TINYREFL_TYPE_SAFE_VERSION)
    set(TINYREFL_TYPE_SAFE_VERSION v0.2.1)
endif()
if(NOT TINYREFL_SPDLOG_URL)
    set(TINYREFL_SPDLOG_URL "https://github.com/gabime/spdlog")
endif()
if(NOT TINYREFL_SPDLOG_VERSION)
    set(TINYREFL_SPDLOG_VERSION v1.8.5)
endif()
if(NOT TINYREFL_PROTOBUF_URL)
    set(TINYREFL_PROTOBUF_URL "https://github.com/protocolbuffers/protobuf")
endif()
if(NOT TINYREFL_PROTOBUF_VERSION)
    set(TINYREFL_PROTOBUF_VERSION v3.15.5)
endif()

if(TINYREFL_TOOL_USING_CONAN_TARGETS)
    message(STATUS "tinyrefl-tool build using conan targets")

    add_library(tinyrefl_externals_fmt INTERFACE)
    add_library(tinyrefl_externals_cppast INTERFACE)
    add_library(tinyrefl_externals_llvm_support INTERFACE)
    add_library(tinyrefl_externals_type_safe INTERFACE)
    add_library(tinyrefl_externals_spdlog INTERFACE)
    add_library(tinyrefl_externals_protobuf INTERFACE)

    target_link_libraries(tinyrefl_externals_fmt INTERFACE CONAN_PKG::fmt)
    target_link_libraries(tinyrefl_externals_cppast INTERFACE CONAN_PKG::cppast)
    target_link_libraries(tinyrefl_externals_llvm_support
                          INTERFACE CONAN_PKG::llvm_support)
    target_link_libraries(tinyrefl_externals_type_safe
                          INTERFACE CONAN_PKG::type_safe)
    target_link_libraries(tinyrefl_externals_spdlog INTERFACE CONAN_PKG::spdlog)
    target_link_libraries(tinyrefl_externals_protobuf
                          INTERFACE CONAN_PKG::protobuf)

    # TODO: Create conan recipe for cppfs
    external_dependency(cppfs ${TINYREFL_CPPFS_REPO_URL}
                        ${TINYREFL_CPPFS_VERSION})
    add_library(tinyrefl_externals_cppfs ALIAS cppfs)
else()
    find_package(fmt QUIET)
    find_package(cppast QUIET)
    find_package(llvm_support QUIET)
    find_package(cppfs QUIET)
    find_package(type_safe QUIET)
    find_package(spdlog QUIET)
    find_package(Protobuf QUIET)

    if(fmt_FOUND)
        message(
            STATUS "tinyrefl-tool build using fmt from find_package() module")

        add_library(tinyrefl_externals_fmt INTERFACE)
        target_link_libraries(tinyrefl_externals_fmt INTERFACE fmt::fmt)
    else()
        message(STATUS "tinyrefl-tool build using fmt from sources")

        external_dependency(fmt-header-only ${TINYREFL_FMT_REPO_URL}
                            ${TINYREFL_FMT_VERSION})

        # Here we cannot define an ALIAS library since fmt::fmt-header-only
        # itself is already an alias
        add_library(tinyrefl_externals_fmt INTERFACE)
        target_link_libraries(tinyrefl_externals_fmt
                              INTERFACE fmt::fmt-header-only)
    endif()

    if(NOT cppast_FOUND OR NOT llvm_support_FOUND)
        message(STATUS "tinyrefl-tool build using cppast from sources")

        if(TINYREFL_USE_LOCAL_LLVM)
            # Find local llvm-config tool for cppast setup
            find_program(
                llvm-config
                NAMES
                    llvm-config
                    llvm-config-${TINYREFL_LLVM_VERSION_MAJOR}.${TINYREFL_LLVM_VERSION_MINOR}
            )

            if(llvm-config)
                execute_process(
                    COMMAND ${llvm-config} --version
                    OUTPUT_VARIABLE llvm-config-version
                    OUTPUT_STRIP_TRAILING_WHITESPACE)

                if(llvm-config-version VERSION_EQUAL TINYREFL_LLVM_VERSION)
                    set(LLVM_CONFIG_BINARY "${llvm-config}")
                    message(
                        STATUS
                            "Using local LLVM ${TINYREFL_LLVM_VERSION} install")
                else()
                    message(
                        FATAL_ERROR
                            "Wrong LLVM install found. Found llvm-config ${llvm-config-version}, required ${TINYREFL_LLVM_VERSION}"
                    )
                endif()
            else()
                message(
                    FATAL_ERROR
                        "TINYREFL_USE_LOCAL_LLVM set and llvm-config program not found"
                )
            endif()
        else()
            # Download precompiled LLVM release
            if(NOT TINYREFL_LLVM_DOWNLOAD_URL)
                if(TINYREFL_LLVM_DOWNLOAD_FROM_OFFICIAL_SERVER)
                    message(
                        STATUS
                            "Using default LLVM download url from LLVM official servers"
                    )

                    if(TINYREFL_LLVM_VERSION_MAJOR EQUAL 5)
                        set(TINYREFL_LLVM_DOWNLOAD_URL
                            "http://releases.llvm.org/${TINYREFL_LLVM_VERSION}/clang+llvm-${TINYREFL_LLVM_VERSION}-linux-x86_64-ubuntu14.04.tar.xz"
                        )
                    else()
                        set(TINYREFL_LLVM_DOWNLOAD_URL
                            "http://releases.llvm.org/${TINYREFL_LLVM_VERSION}/clang+llvm-${TINYREFL_LLVM_VERSION}-x86_64-linux-gnu-ubuntu-14.04.tar.xz"
                        )
                    endif()
                else()
                    message(
                        STATUS "Using default LLVM download url from mirror")
                    set(TINYREFL_LLVM_DOWNLOAD_URL
                        "https://llvm-releases.s3.us-east-2.amazonaws.com/clang%2Bllvm-${TINYREFL_LLVM_VERSION}-x86_64-linux-gnu-ubuntu-14.04.tar.xz"
                    )
                endif()
            else()
                message(
                    STATUS
                        "Using custom LLVM download url: ${TINYREFL_LLVM_DOWNLOAD_URL}"
                )
            endif()

            # LLVM releases are compiled with old GCC ABI
            add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
            # Force cppast to be compiled with old GCC ABI
            set(CPPAST_USE_OLD_LIBSTDCPP_ABI
                ON
                CACHE INTERNAL "")

            set(LLVM_DOWNLOAD_URL "${TINYREFL_LLVM_DOWNLOAD_URL}")
            message(
                STATUS "Using LLVM download URL: ${TINYREFL_LLVM_DOWNLOAD_URL}")
        endif()

        external_dependency(cppast ${TINYREFL_CPPAST_REPO_URL}
                            ${TINYREFL_CPPAST_VERSION})
        add_library(tinyrefl_externals_cppast ALIAS cppast)
        add_library(tinyrefl_externals_type_safe ALIAS type_safe)

        if(NOT LLVM_CONFIG_BINARY)
            message(FATAL_ERROR "llvm-config binary not set")
        else()
            message(STATUS "llvm-config binary: ${LLVM_CONFIG_BINARY}")
        endif()

        execute_process(COMMAND ${LLVM_CONFIG_BINARY} --libdir
                        OUTPUT_VARIABLE stdout)
        string(STRIP "${stdout}" stdout)
        set(LLVM_CMAKE_PATH
            "${stdout}/cmake/llvm"
            CACHE PATH "")
        set(CLANG_CMAKE_PATH
            "${stdout}/cmake/clang"
            CACHE PATH "")
        set(TINYREFL_TOOL_BUILDING_CPPAST_FROM_SOURCES TRUE)

        message(STATUS "llvm cmake path: ${LLVM_CMAKE_PATH}")
        message(STATUS "clang cmake path: ${CLANG_CMAKE_PATH}")

        find_package(
            LLVM
            ${TINYREFL_LLVM_VERSION}
            REQUIRED
            EXACT
            CONFIG
            PATHS
            "${LLVM_CMAKE_PATH}"
            NO_DEFAULT_PATH)
        add_library(tinyrefl_externals_llvm_support INTERFACE)
        target_link_libraries(tinyrefl_externals_llvm_support
                              INTERFACE LLVMSupport)
    else()
        if(cppast_FOUND)
            message(
                STATUS
                    "tibyrefl-tool build using cppast from find_package() module"
            )

            add_library(tinyrefl_externals_cppast INTERFACE)
            target_link_libraries(tinyrefl_externals_cppast
                                  INTERFACE cppast::cppast)
        else()
            message(FATAL_ERROR "cppast library dependency not found")
        endif()

        if(llvm_support_FOUND)
            message(
                STATUS
                    "tinyrefl-tool build using llvm_support from find_package() module"
            )
            message(
                STATUS
                    "tinyrefl-tool build using cppast from find_package() module"
            )

            add_library(tinyrefl_externals_llvm_support INTERFACE)
            target_link_libraries(tinyrefl_externals_llvm_support
                                  INTERFACE llvm_support::llvm_support)

            if(NOT (TINYREFL_LLVM_VERSION STREQUAL llvm_support_VERSION))
                message(
                    FATAL_ERROR
                        "Found llvm version (${llvm_support_VERSION}) does not match configured TINYREFL_LLVM_VERSION (${TINYREFL_LLVM_VERSION})"
                )
            endif()

        else()
            message(FATAL_ERROR "LLVMSupport library dependency not found")
        endif()
    endif()

    if(NOT cppfs_FOUND)
        message(STATUS "tinyrefl-tool build using cppfs from sources")

        external_dependency(cppfs ${TINYREFL_CPPFS_REPO_URL}
                            ${TINYREFL_CPPFS_VERSION})
        add_library(tinyrefl_externals_cppfs ALIAS cppfs)
    else()
        message(
            STATUS "tinyrefl-tool build using cppfs from find_package() module")

        add_library(tinyrefl_externals_cppfs INTERFACE)
        target_link_libraries(tinyrefl_externals_cppfs INTERFACE cppfs::cppfs)
    endif()

    if(NOT TARGET tinyrefl_externals_type_safe)
        if(NOT type_safe_FOUND)
            message(STATUS "tinyrefl-tool build using type_safe from sources")

            external_dependency(type_safe ${TINYREFL_TYPE_SAFE_URL}
                                ${TINYREFL_TYPE_SAFE_VERSION})
            add_library(tinyrefl_externals_type_safe ALIAS type_safe)
        else()
            message(
                STATUS
                    "tinyrefl-tool build using type_safe from find_package() module"
            )

            add_library(tinyrefl_externals_type_safe INTERFACE)
            target_link_libraries(tinyrefl_externals_type_safe
                                  INTERFACE type_safe::type_safe)
        endif()
    endif()

    if(NOT spdlog_FOUND)
        message(STATUS "tinyrefl-tool build using spdlog from sources")

        set(SPDLOG_INSTALL
            OFF
            CACHE INTERNAL "")
        external_dependency(spdlog ${TINYREFL_SPDLOG_URL}
                            ${TINYREFL_SPDLOG_VERSION})
        add_library(tinyrefl_externals_spdlog ALIAS spdlog)
    else()
        message(
            STATUS "tinyrefl-tool build using spdlog from find_package() module"
        )

        add_library(tinyrefl_externals_spdlog INTERFACE)
        target_link_libraries(tinyrefl_externals_spdlog
                              INTERFACE spdlog::spdlog)
    endif()

    if(NOT protobuf_FOUND)
        message(STATUS "tinyrefl-tool build using protobuf from sources")

        set(PROTOBUF_INSTALL
            OFF
            CACHE INTERNAL "")
        external_dependency(protobuf ${TINYREFL_PROTOBUF_URL}
                            ${TINYREFL_PROTOBUF_VERSION} SOURCE_SUBDIR cmake)
        add_library(tinyrefl_externals_protobuf ALIAS libprotobuf)
        add_executable(tinyrefl_externals_protoc ALIAS protoc)
        include(${CMAKE_CURRENT_LIST_DIR}/protobuf_generate.cmake)
    else()
        message(
            STATUS
                "tinyrefl-tool build using protobuf from find_package() module")

        add_library(tinyrefl_externals_protobuf INTERFACE)
        target_link_libraries(tinyrefl_externals_protobuf
                              INTERFACE protobuf::libprotobuf)
    endif()
endif()

function(define_llvm_version_variables TARGET)
    add_variable_compile_definition(${TARGET} TINYREFL_LLVM_VERSION STRING)
    add_variable_compile_definition(${TARGET} TINYREFL_LLVM_VERSION_MAJOR)
    add_variable_compile_definition(${TARGET} TINYREFL_LLVM_VERSION_MINOR)
    add_variable_compile_definition(${TARGET} TINYREFL_LLVM_VERSION_FIX)
    add_variable_compile_definition(${TARGET} TINYREFL_LLVM_VERSION_MAJOR
                                    SUFFIX _STRING STRING)
    add_variable_compile_definition(${TARGET} TINYREFL_LLVM_VERSION_MINOR
                                    SUFFIX _STRING STRING)
    add_variable_compile_definition(${TARGET} TINYREFL_LLVM_VERSION_FIX SUFFIX
                                    _STRING STRING)
endfunction()

function(define_tinyrefl_deps_version_variables TARGET)
    define_llvm_version_variables(${TARGET})

    add_variable_compile_definition(${TARGET} TINYREFL_CPPAST_REPO_URL STRING)
    add_variable_compile_definition(${TARGET} TINYREFL_CPPAST_VERSION STRING)
    add_variable_compile_definition(${TARGET} TINYREFL_PROTOBUF_VERSION STRING)
endfunction()
