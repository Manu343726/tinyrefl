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
    get_target_dependencies_impl(${TARGET} ${TARGET} ${RESULT} "")
    set(${RESULT} ${${RESULT}} PARENT_SCOPE)
endfunction()

function(get_target_dependencies_impl TARGET ROOT_TARGET RESULT CALLSTACK)
    if(NOT TARGET ${TARGET})
        set(notatarget "[NOT A TARGET!!!] ")
    endif()

    list(LENGTH CALLSTACK stack_length)
    string(REGEX REPLACE ";" " -> " stack_string "${CALLSTACK}")

    if(stack_length GREATER 0)
        string(RANDOM LENGTH ${stack_length} ALPHABET " " padding)
    else()
        set(padding "")
    endif()

    if(TINYREFL_UTILS_DEBUG_GETTARGETDEPENDENCIES)
        message(STATUS "${padding}[${stack_length}] ${TARGET} ${notatarget}(from '${ROOT_TARGET}': ${stack_string})")
    endif()

    if(NOT TARGET ${TARGET})
        set(${RESULT} PARENT_SCOPE)
        return()
    endif()

    list(APPEND CALLSTACK ${TARGET})

    if(("${TARGET}" STREQUAL "${ROOT_TARGET}") AND (stack_length GREATER 0))
        string(REGEX REPLACE ";" " -> " stack_string "${CALLSTACK}")
        message(FATAL_ERROR "Dependency cycle detected on ${ROOT_TARGET} target. Full dependency chain: ${stack_string}")
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
            get_target_dependencies_impl(${dep} ${ROOT_TARGET} deps "${CALLSTACK}")
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

        if(TINYREFL_VERBOSE_CONFIG)
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

function(clangxx_stdlib_includes stdlib INCLUDES)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(clangxx ${CMAKE_CXX_COMPILER})
        list(APPEND compilers clangxx)
    elseif(CMAKE_COMPILER_IS_GNUCXX)
	set(gxx ${CMAKE_CXX_COMPILER})
        list(APPEND compilers gxx)
    elseif(MSVC)
        clangxx_executable(clangxx)
        list(APPEND compilers clangxx)
    else()
        message(FATA_ERROR "Unsupported compiler")
    endif()

    foreach(compiler_exec ${compilers})
        # We want DRLParser to work even if MinGW is not available on Windows,
        # but note that would add some innaccuracies when parsing with libclang
        # (Missing standard library headers, etc)
        if(NOT ${compiler_exec})
            continue()
        endif()

        if(TINYREFL_VERBOSE_CONFIG)
            message(STATUS "Asking for ${${compiler_exec}} (${compiler_exec}) include dirs...")
        endif()
        if(compiler_exec MATCHES "clangxx")
            set(stdlib_option "-stdlib=${stdlib}")
        endif()

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${${compiler_exec}} -std=c++11 ${stdlib_option} -v -x c++ -E -
            OUTPUT_VARIABLE out
            ERROR_VARIABLE err
        )

        if(TINYREFL_VERBOSE_CONFIG)
            message(STATUS "output from ${${compiler_exec}}: ${out}")
            message(STATUS "error output from ${${compiler_exec}}: ${err}")
        endif()

        # Since GNU has localization in most of their tools (GCC included)
        # the diagnostics are translated to the different user languages.
        # We have to handle the different languages...
        #
        # The fun thing of this is that GNU translation to Spanish
        # seems to be done by a bad Google Translate bot or something

        set(locale_component_separator "__siplasplas_locale_component_separator__")

        set(include_search_locales
            "english
${locale_component_separator}
#include \"...\" search starts here:
#include <...> search starts here:
${locale_component_separator}
End of search list."

            "spanish
${locale_component_separator}
la búsqueda de #include \"...\" inicia aquí:
la búsqueda de #include <...> inicia aquí:
${locale_component_separator}
Fin de la lista de búsqueda"
        )

        foreach(locale ${include_search_locales})
            string(REGEX REPLACE "(.*)\n${locale_component_separator}\n.*\n${locale_component_separator}\n.*" "\\1" language "${locale}")
            string(REGEX REPLACE ".*\n${locale_component_separator}\n(.*)\n${locale_component_separator}\n.*" "\\1" header "${locale}")
            string(REGEX REPLACE ".*\n${locale_component_separator}\n.*\n${locale_component_separator}\n(.*)" "\\1" footer "${locale}")

            if(TINYREFL_VERBOSE_CONFIG)
                message(STATUS "Checking ${${compiler_exec}} output with ${language} language...")
                message(STATUS " - header: ${header}")
                message(STATUS " - footer: ${footer}")
            endif()

            string(REGEX MATCH "
${header}
(.+)
${footer}"
                    includes "${err}\n STDOUT: ${out}")

            string(REGEX REPLACE "\n" ";" includes "${CMAKE_MATCH_1}")
            list(REMOVE_DUPLICATES includes)

            foreach(includedir ${includes})
                if(WIN32)
                    windows_path("${includedir}" includedir)
                endif()

                string(REGEX REPLACE "\ +$" "" includedir "${includedir}")
                string(REGEX REPLACE "\"(.*)\"" "\\1" includedir "${includedir}")
                string(REGEX REPLACE "\n" "" includedir "${includedir}")
                string(STRIP "${includedir}" includedir)

                list(APPEND includedirs ${includedir})
            endforeach()
        endforeach()

        if(includedirs)
            if(WIN32)
                # Explicitly add MinGW libstdc++ include dir
                gxx_version(gxx_version)
                list(APPEND includedirs
                    "C:/MinGW/include/c++/${gxx_version}"
                    "C:/MinGW/include/c++/${gxx_version}/x86_64-w64-mingw32"
                )
            endif()

	    if(includedirs)
		list(REMOVE_DUPLICATES includedirs)
		set(${INCLUDES} "${includedirs}" PARENT_SCOPE)
	    else()
		set(${INCLUDES} PARENT_SCOPE)
	    endif()

            return()
        endif()
    endforeach()
endfunction()

function(add_variable_compile_definition TARGET VARIABLE)
    cmake_parse_arguments(
        PREFIX
        "STRING"
        "PREFIX;SUFFIX"
        ""
        ${ARGN}
    )

    message(STATUS "TARGET ${TARGET} compile definition ${VARIABLE}=${${VARIABLE}}")

    if(PREFIX_STRING)
        target_compile_definitions(${TARGET} PUBLIC ${PREFIX_PREFIX}${VARIABLE}${PREFIX_SUFFIX}="${${VARIABLE}}")
    else()
        target_compile_definitions(${TARGET} PUBLIC ${PREFIX_PREFIX}${VARIABLE}${PREFIX_SUFFIX}=${${VARIABLE}})
    endif()
endfunction()

function(mark_as_external_target TARGET)
    get_property(type TARGET ${TARGET} PROPERTY TYPE)

    if(NOT (type STREQUAL "INTERFACE_LIBRARY"))
        get_property(includes TARGET ${TARGET} PROPERTY INCLUDE_DIRECTORIES)
        set_property(TARGET ${TARGET} APPEND PROPERTY SYSTEM_INCLUDE_DIRECTORIES ${includes})
    endif()

    get_property(interface_includes TARGET ${TARGET} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    set_property(TARGET ${TARGET} APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES  ${interface_includes})
endfunction()
