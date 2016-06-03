set(_mason_command ${CMAKE_SOURCE_DIR}/.mason/mason)
string(RANDOM LENGTH 16 _mason_invocation)

if(EXISTS "${_mason_command}")
else()
   execute_process(
        COMMAND git submodule update --init .mason
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

function(_mason_valid_args package version)
    if(NOT package OR NOT version)
        message(FATAL_ERROR "No package name or version given")
    endif()
endfunction()

function(_mason_install package version)
    set(_mason_failed)
    execute_process(
        COMMAND ${_mason_command} install ${package} ${version}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE _mason_failed)
    if(_mason_failed)
        message(FATAL_ERROR "[Mason] Could not install Mason package")
    endif()
endfunction()

function(_mason_get_flags package version)
   set(_mason_flags)
   set(_mason_failed)
   execute_process(
        COMMAND ${_mason_command} cflags ${package} ${version}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE _mason_flags
        RESULT_VARIABLE _mason_failed
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(_mason_failed)
        message(FATAL_ERROR "[Mason] Could not get flags for package ${package} ${version}")
    endif()

    # Extract -I and -isystem to {package}_INCLUDE_DIRS
    string(REGEX MATCHALL "(^| +)-(I|isystem) *([^ ]+)" _mason_include_dirs "${_mason_flags}")
    string(REGEX REPLACE "(^| +)-(I|isystem) *" "" _mason_include_dirs "${_mason_include_dirs}")
    string(STRIP "${_mason_include_dirs}" _mason_include_dirs)
    list(REMOVE_DUPLICATES _mason_include_dirs)
    set(_mason_include_dirs "${_mason_include_dirs}" PARENT_SCOPE)

    # Extract -D definitions to {package}_DEFINITIONS
    string(REGEX MATCHALL "(^| +)-D *([^ ]+)" _mason_definitions "${_mason_flags}")
    string(REGEX REPLACE "(^| +)-D *" "\\1" _mason_definitions "${_mason_definitions}")
    string(STRIP "${_mason_definitions}" _mason_definitions)
    set(_mason_definitions "${_mason_definitions}" PARENT_SCOPE)

    # Store all other flags in {package}_OPTIONS
    string(REGEX REPLACE "(^| +)-(D|I|isystem) *([^ ]+)" "" _mason_options "${_mason_flags}")
    string(STRIP "${_mason_options}" _mason_options)
    set(_mason_options "${_mason_options}" PARENT_SCOPE)
endfunction()

function(_mason_get_libs package version)
    set(_mason_failed)
    set(_mason_static_libs)
    execute_process(
        COMMAND ${_mason_command} static_libs ${package} ${version}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE _mason_static_libs
        RESULT_VARIABLE _mason_failed
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(_mason_failed)
        message(FATAL_ERROR "[Mason] Could not get static libraries for package ${package} ${version}")
    endif()

    set(_mason_failed)
    set(_mason_ldflags)
    execute_process(
        COMMAND ${_mason_command} ldflags ${package} ${version}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE _mason_ldflags
        RESULT_VARIABLE _mason_failed
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(_mason_failed)
        message(FATAL_ERROR "[Mason] Could not get linker flags for package ${package} ${version}")
    endif()

    set(_mason_libraries)
    list(APPEND _mason_libraries ${_mason_static_libs} ${_mason_ldflags})
    set(_mason_libraries ${_mason_libraries} PARENT_SCOPE)
endfunction()

macro(mason_use package version)
    _mason_valid_args("${package}" "${version}")

    if(MASON_${package}_INVOCATION STREQUAL ${_mason_invocation})
        # Check that the previous invocation of mason_use didn't select another version of this package
        if(NOT MASON_${package}_VERSION STREQUAL ${version})
            message(FATAL_ERROR "[Mason] Already using ${package} ${MASON_${package}_VERSION}. Cannot select version ${version}.")
        endif()
    else()
        _mason_install("${package}" "${version}")
        _mason_get_flags("${package}" "${version}")
        _mason_get_libs("${package}" "${version}")
        set(MASON_${package}_INCLUDE_DIRS "${_mason_include_dirs}" CACHE STRING "${package} include directories" FORCE)
        set(MASON_${package}_DEFINITIONS "${_mason_definitions}" CACHE STRING "${package} definitions" FORCE)
        set(MASON_${package}_OPTIONS "${_mason_options}" CACHE STRING "${package} compiler options" FORCE)
        set(MASON_${package}_LIBRARIES "${_mason_libraries}" CACHE STRING "${package} libraries" FORCE)
        set(MASON_${package}_VERSION "${version}" CACHE INTERNAL "${package} version" FORCE)
        set(MASON_${package}_INVOCATION "${_mason_invocation}" CACHE INTERNAL "${package} invocation ID" FORCE)
    endif()
endmacro()

macro(target_add_mason_package target visibility package)
    mason_use("${package}" "${MASON_${package}_VERSION}")
    target_include_directories(${target} ${visibility} "${MASON_${package}_INCLUDE_DIRS}")
    target_compile_definitions(${target} ${visibility} "${MASON_${package}_DEFINITIONS}")
    target_compile_options(${target} ${visibility} "${MASON_${package}_OPTIONS}")
    target_link_libraries(${target} ${visibility} "${MASON_${package}_LIBRARIES}")
endmacro()
