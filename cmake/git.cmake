find_package(Git REQUIRED)

function(git_branch RESULT)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE stdout
        RESULT_VARIABLE res
        WORKING_DIRECTORY "${TINYREFL_SOURCE_DIR}"
    )

    string(STRIP "${stdout}" stdout)

    if(NOT (res EQUAL 0))
        message(FATAL_ERROR "Could not get git branch")
    endif()

    set(${RESULT} "${stdout}" PARENT_SCOPE)
endfunction()

function(git_commit_hash RESULT)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        OUTPUT_VARIABLE stdout
        RESULT_VARIABLE res
        WORKING_DIRECTORY "${TINYREFL_SOURCE_DIR}"
    )

    string(STRIP "${stdout}" stdout)

    if(NOT (res EQUAL 0))
        message(FATAL_ERROR "Could not get git commit hash")
    endif()

    set(${RESULT} "${stdout}" PARENT_SCOPE)
endfunction()

function(git_current_checkout_is_tag RESULT)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --exact-match HEAD
        OUTPUT_VARIABLE stdout
        ERROR_VARIABLE stderr
        RESULT_VARIABLE res
        WORKING_DIRECTORY "${TINYREFL_SOURCE_DIR}"
    )

    string(STRIP "${stdout}" stdout)

    if(res EQUAL 0)
        set(${RESULT} TRUE PARENT_SCOPE)
    else()
        set(${RESULT} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(git_last_tag RESULT)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --abbrev=0
        OUTPUT_VARIABLE stdout
        RESULT_VARIABLE res
        WORKING_DIRECTORY "${TINYREFL_SOURCE_DIR}"
    )

    string(STRIP "${stdout}" stdout)

    if(NOT (res EQUAL 0))
        message(FATAL_ERROR "Could not get last git tag")
    endif()

    set(${RESULT} "${stdout}" PARENT_SCOPE)
endfunction()

git_branch(TINYREFL_GIT_BRANCH)
git_commit_hash(TINYREFL_GIT_COMMIT_HASH)
git_current_checkout_is_tag(TINYREFL_GIT_AT_TAG)
git_last_tag(TINYREFL_GIT_LAST_TAG)

string(REGEX REPLACE "v([0-9]+).([0-9]+).([0-9]+)" "\\1" TINYREFL_VERSION_MAJOR "${TINYREFL_GIT_LAST_TAG}")
string(REGEX REPLACE "v([0-9]+).([0-9]+).([0-9]+)" "\\2" TINYREFL_VERSION_MINOR "${TINYREFL_GIT_LAST_TAG}")
string(REGEX REPLACE "v([0-9]+).([0-9]+).([0-9]+)" "\\3" TINYREFL_VERSION_FIX   "${TINYREFL_GIT_LAST_TAG}")

string(STRIP "${TINYREFL_VERSION_MAJOR}" TINYREFL_VERSION_MAJOR)
string(STRIP "${TINYREFL_VERSION_MINOR}" TINYREFL_VERSION_MINOR)
string(STRIP "${TINYREFL_VERSION_FIX}" TINYREFL_VERSION_FIX)

message(STATUS "Tinyrefl version major: ${TINYREFL_VERSION_MAJOR}")
message(STATUS "Tinyrefl version minor: ${TINYREFL_VERSION_MINOR}")
message(STATUS "Tinyrefl version fix: ${TINYREFL_VERSION_FIX}")
set(TINYREFL_VERSION "${TINYREFL_VERSION_MAJOR}.${TINYREFL_VERSION_MINOR}.${TINYREFL_VERSION_FIX}")
message(STATUS "Tinyrefl version ${TINYREFL_VERSION}")
message(STATUS "Tinyrefl git branch: ${TINYREFL_GIT_BRANCH}")
message(STATUS "Tinyrefl git commit: ${TINYREFL_GIT_COMMIT_HASH}")
message(STATUS "Tinyrefl at git tag: ${TINYREFL_GIT_AT_TAG}")
message(STATUS "Tinyrefl latest git tag: ${TINYREFL_GIT_LAST_TAG}")

function(define_tinyrefl_version_variables TARGET)
    target_compile_definitions(${TARGET} PUBLIC
        TINYREFL_VERSION="${TINYREFL_VERSION}"
        TINYREFL_VERSION_MAJOR=${TINYREFL_VERSION_MAJOR}
        TINYREFL_VERSION_MINOR=${TINYREFL_VERSION_MINOR}
        TINYREFL_VERSION_FIX=${TINYREFL_VERSION_FIX}
        TINYREFL_GIT_COMMIT="${TINYREFL_GIT_COMMIT_HASH}"
        TINYREFL_GIT_BRANCH="${TINYREFL_GIT_BRANCH}"
    )
endfunction()
