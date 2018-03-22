# Returns the relative path of the
# current source dir against the source
# tree root.
#
function(sourcetree_relative RESULT)
    file(RELATIVE_PATH relpath "${CMAKE_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}")
    set(${RESULT} "${relpath}" PARENT_SCOPE)
endfunction()

# Gets the headers directory equivalent to the
# current source subtree.
#
#
# The library is organized in two directories:
#
# - Headers: Root at <source-dir>/include/siplasplas/,
#    one directory per module/library target.
#
# - Sources: The source tree, with root at <source-dir>/src/,
#   one subdirectory per module/library. This is where
#   CMakeLists.txt files are.
#
# Given a library target organised in that way, the function returns
# the path to the headers folder of the target/s being processed in this
# sourcetree location..
#
function(headerdir_from_sourcetree RESULT)
    sourcetree_relative(relpath)
    set(includedir "${CMAKE_SOURCE_DIR}/include/siplasplas/${relpath}")
    set(${RESULT}  "${includedir}" PARENT_SCOPE)
endfunction()

# Unzips a library list so libraries tagged as
# 'debug', 'optimized', or 'general' are arranged into
# isolated lists.
#
# When passing libraries to commands like target_link_libraries()
# CMake tags them so the linker/user may know which of these libraries
# should be used in several compilation modes.
#
#
# Script took from https://software.lanl.gov/MeshTools/trac/browser/cmake/modules/ParseLibraryList.cmake
function(parse_library_list LIBRARIES)
    foreach( item ${LIBRARIES} )
        if( ${item} MATCHES debug     OR
            ${item} MATCHES optimized OR
            ${item} MATCHES general )

            if( ${item} STREQUAL "debug" )
                set( mylist "_debug_libs" )
            elseif( ${item} STREQUAL "optimized" )
                set( mylist "_opt_libs" )
            elseif( ${item} STREQUAL "general" )
                set( mylist "_gen_libs" )
            endif()
      else()
          list( APPEND ${mylist} ${item} )
      endif()
    endforeach()

    set(DEBUG_LIBS     ${_debug_libs}     PARENT_SCOPE)
    set(OPTIMIZED_LIBS ${_optimized_libs} PARENT_SCOPE)
    set(GENERAL_LIBS   ${_gen_libs}       PARENT_SCOPE)
endfunction()

function(get_target_common_property RESULT TARGET PROPERTY)
    if(NOT TARGET ${TARGET})
        set(${RESULT} PARENT_SCOPE)
        return()
    endif()

    get_target_property(type ${TARGET} TYPE)

    if(type STREQUAL "INTERFACE_LIBRARY")
        get_target_property(value ${TARGET} INTERFACE_${PROPERTY})
    else()
        get_target_property(interface-value ${TARGET} INTERFACE_${PROPERTY})
        get_target_property(value ${TARGET} ${PROPERTY})

        if(interface-value)
            if(value)
                set(value ${value} ${interface-value})
            else()
                set(value ${interface-value})
            endif()
        endif()
    endif()

    if(NOT value)
        set(value) # clear -NOTFOUND entries
    else()
        list(REMOVE_DUPLICATES value)
    endif()

    set(${RESULT} ${value} PARENT_SCOPE)
endfunction() 

function(get_target_compile_options TARGET RESULT)
    if(NOT TARGET ${TARGET})
        set(${RESULT} PARENT_SCOPE)
        return()
    endif()

    get_target_common_property(compile_options ${TARGET} COMPILE_OPTIONS)
    get_target_dependencies(${TARGET} dependencies)

    foreach(dep ${dependencies})
        get_target_common_property(dep_compile_options ${dep} COMPILE_OPTIONS)
        list(APPEND compile_options ${dep_compile_options})
    endforeach()

    if(compile_options)
        list(REMOVE_DUPLICATES compile_options)
    endif()
    set(${RESULT} ${compile_options} PARENT_SCOPE)
endfunction()

# Gets the set of target include directories, recusivelly scanning
# dependencies and checking kind of target
function(get_target_include_directories TARGET RESULT)
    if(NOT TARGET ${TARGET})
        set(${RESULT} PARENT_SCOPE)
        return()
    endif()

    get_target_common_property(includes ${TARGET} INCLUDE_DIRECTORIES)
    get_target_dependencies(${TARGET} dependencies)

    foreach(dep ${dependencies})
        get_target_common_property(dep_includes ${dep} INCLUDE_DIRECTORIES)
        list(APPEND includes ${dep_includes})
    endforeach()

    if(includes)
        list(REMOVE_DUPLICATES includes)
    endif()
    set(${RESULT} ${includes} PARENT_SCOPE)
endfunction()

# Gets a list of target dependencies
function(get_target_dependencies TARGET RESULT)
    if(NOT TARGET ${TARGET})
        set(${RESULT} PARENT_SCOPE)
        return()
    endif()

    get_target_common_property(link_libraries ${TARGET} LINK_LIBRARIES)

    if(link_libraries)
        set(deps_copy ${link_libraries})
        set(dependencies)
        foreach(dep ${deps_copy})
            if(NOT (dep STREQUAL "${TARGET}"))
                list(APPEND dependencies "${dep}")
            endif()
        endforeach()

        foreach(dep ${dependencies})
            get_target_dependencies(${dep} deps)
            list(APPEND dependencies ${deps})
        endforeach()
    endif()

    if(dependencies)
        list(REMOVE_DUPLICATES dependencies)
    else()
        # clear variable (May be evaluated to false by -NOTFOUND value)
        set(dependencies)
    endif()
    set(${RESULT} ${dependencies} PARENT_SCOPE)
endfunction()

function(get_target_binary TARGET RESULT)
    if(NOT TARGET ${TARGET})
        message(FATAL_ERROR "${TARGET} is not a CMake target")
    endif()

    get_target_property(type ${TARGET} TYPE)

    if(type MATCHES INTERFACE_LIBRARY)
        message(FATAL_ERROR "Interface libraries have no binary output (Library: ${TARGET})")
    else()
        set(binary "$<TARGET_FILE_DIR:${TARGET}>/$<TARGET_FILE_NAME:${TARGET}>")
    endif()

    if(binary)
        set(${RESULT} "${binary}" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Failed getting binary of target ${TARGET}")
    endif()
endfunction()

function(get_target_dependencies_targets_only TARGET RESULT)
    get_target_dependencies(${TARGET} dependencies)

    foreach(dep ${dependencies})
        if(TARGET ${dep})
            list(APPEND target_dependencies ${dep})
        endif()
    endforeach()

    set(${RESULT} ${target_dependencies} PARENT_SCOPE)
endfunction()

function(add_prebuild_command)
    set(options)
    set(oneValueArgs NAME TARGET)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(PC
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(NOT PC_TARGET)
        message(FATAL_ERROR "No target specified for pre build command")
    endif()

    if(NOT PC_NAME)
        set(PC_NAME "${PC_TARGET}_prebuild")
    endif()

    # First create a dummy target to link the command as POST_BUILD
    add_custom_target(${PC_NAME})
    add_custom_command(TARGET ${PC_NAME} POST_BUILD ${PC_UNPARSED_ARGUMENTS})

    # Here we force the command target to be the uppermost dependency of the TARGET, so
    # the command is run only after building all other TARGET dependencies
    get_target_dependencies_targets_only(${PC_TARGET} deps)
    if(deps)
        add_dependencies(${PC_NAME} ${deps})
    endif()
    add_dependencies(${PC_TARGET} ${PC_NAME})

    # Build the explicit dependencies first:
    if(PC_DEPENDS)
        add_dependencies(${PC_NAME} ${PC_DEPENDS})
    endif()
endfunction()

# Copies dll dependencies of a target to its runtime
# directory.
#
# Recursivelly scans for dll dependencies, so all needed
# dlls are placed within the target output binary (usually
# an executable). This helps to run/debug executables directly
# in the buildtree, without installing them.
#
function(copy_dll_dependencies TARGET)
    get_target_dependencies(${TARGET} dependencies)

    set(dest_directory $<TARGET_FILE_DIR:${TARGET}>)
    foreach(dep ${dependencies})
        if(TARGET ${dep})
            get_target_property(type ${dep} TYPE)

            if(NOT type MATCHES INTERFACE_LIBRARY)
                get_target_binary(${dep} lib)
                set(dllfile "${lib}")
            else()
                set(dllfile)
            endif()
        else()
            set(dllfile)
        endif()

        if(SIPLASPLAS_VERBOSE_CONFIG)
            set(log COMMAND ${CMAKE_COMMAND} -E echo " - ${dllfile}")
        endif()

        if(dllfile)
            list(APPEND copy_command
                ${log}
                COMMAND ${CMAKE_COMMAND} -E copy
                    \"${dllfile}\"
                    \"${dest_directory}\"
            )
        endif()
    endforeach()

    if(copy_command)
        add_custom_target(copy-${TARGET}-dlls
            ${copy_command}
            COMMENT "Copying dll dependencies of target ${TARGET}..."
        )
    endif()
endfunction()

function(parse_version_number NUMBER MAJOR MINOR FIX)
    string(REGEX REPLACE "([0-9])+\\.([0-9]+)\\.([0-9]+)" "\\1" _major "${NUMBER}")
    string(REGEX REPLACE "([0-9])+\\.([0-9]+)\\.([0-9]+)" "\\2" _minor "${NUMBER}")
    string(REGEX REPLACE "([0-9])+\\.([0-9]+)\\.([0-9]+)" "\\3" _fix   "${NUMBER}")

    set(${MAJOR} "${_major}" PARENT_SCOPE)
    set(${MINOR} "${_minor}" PARENT_SCOPE)
    set(${FIX}   "${_fix}"   PARENT_SCOPE)
endfunction()
