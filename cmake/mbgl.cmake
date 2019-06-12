if (NOT MBGL_PLATFORM)
    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        set(MBGL_PLATFORM "macos")
    else()
        set(MBGL_PLATFORM "linux")
    endif()
endif()

if(WITH_NODEJS)
    find_program(NodeJS_EXECUTABLE NAMES nodejs node)
    if (NOT NodeJS_EXECUTABLE)
        message(FATAL_ERROR "Could not find Node.js")
    endif()

    find_program(npm_EXECUTABLE NAMES npm)
    if (NOT npm_EXECUTABLE)
        message(FATAL_ERROR "Could not find npm")
    endif()

    execute_process(
        COMMAND "${NodeJS_EXECUTABLE}" -e "process.stdout.write(process.versions.node)"
        RESULT_VARIABLE _STATUS_CODE
        OUTPUT_VARIABLE NodeJS_VERSION
        ERROR_VARIABLE _STATUS_MESSAGE
    )
    if(NOT _STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Could not detect Node.js version: ${_STATUS_MESSAGE}")
    endif()

    execute_process(
        COMMAND "${NodeJS_EXECUTABLE}" -e "process.stdout.write(process.versions.modules)"
        RESULT_VARIABLE _STATUS_CODE
        OUTPUT_VARIABLE NodeJS_ABI
        ERROR_VARIABLE _STATUS_MESSAGE
    )
    if(NOT _STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Could not detect Node.js ABI version: ${_STATUS_MESSAGE}")
    endif()

    function(_npm_install DIRECTORY NAME ADDITIONAL_DEPS)
        SET(NPM_INSTALL_FAILED FALSE)
        if("${DIRECTORY}/package.json" IS_NEWER_THAN "${DIRECTORY}/node_modules/.${NAME}.stamp")
            message(STATUS "Running 'npm install' for ${NAME}...")
            execute_process(
                COMMAND "${NodeJS_EXECUTABLE}" "${npm_EXECUTABLE}" install --verbose --ignore-scripts
                WORKING_DIRECTORY "${DIRECTORY}"
                RESULT_VARIABLE NPM_INSTALL_FAILED
                OUTPUT_VARIABLE NPM_OUTPUT
                ERROR_VARIABLE NPM_OUTPUT)
            message(STATUS "Finished 'npm install' for ${NAME}...")
            if(NOT NPM_INSTALL_FAILED)
                execute_process(COMMAND ${CMAKE_COMMAND} -E touch "${DIRECTORY}/node_modules/.${NAME}.stamp")
            else()
                message(FATAL_ERROR "NPM install failed:\n${NPM_OUTPUT}")
            endif()
        endif()

        add_custom_command(
            OUTPUT "${DIRECTORY}/node_modules/.${NAME}.stamp"
            COMMAND "${NodeJS_EXECUTABLE}" "${npm_EXECUTABLE}" install --ignore-scripts
            COMMAND ${CMAKE_COMMAND} -E touch "${DIRECTORY}/node_modules/.${NAME}.stamp"
            WORKING_DIRECTORY "${DIRECTORY}"
            DEPENDS ${ADDITIONAL_DEPS} "${DIRECTORY}/package.json"
            COMMENT "Running 'npm install' for ${NAME}...")
    endfunction()

    # Run submodule update
    set(MBGL_SUBMODULES mapbox-gl-js)
    if(MBGL_PLATFORM STREQUAL "ios")
        list(APPEND MBGL_SUBMODULES platform/ios/vendor/mapbox-events-ios)
    endif()
    if(MBGL_PLATFORM STREQUAL "ios" OR MBGL_PLATFORM STREQUAL "macos")
        list(APPEND MBGL_SUBMODULES platform/darwin/docs/theme)
    endif()

    message(STATUS "Updating submodules...")
    execute_process(
        COMMAND git submodule update --init ${MBGL_SUBMODULES}
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/mapbox-gl-js/node_modules")
        # Symlink mapbox-gl-js/node_modules so that the modules that are
        # about to be installed get cached between CI runs.
        execute_process(
             COMMAND ln -sF ../node_modules .
             WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/mapbox-gl-js")
    endif()

    # Add target for running submodule update during builds
    add_custom_target(
        update-submodules ALL
        COMMAND git submodule update --init ${MBGL_SUBMODULES}
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Updating submodules..."
    )
    set_target_properties(update-submodules PROPERTIES FOLDER "Misc")

    # Run npm install for both directories, and add custom commands, and a target that depends on them.
    _npm_install("${CMAKE_SOURCE_DIR}" mapbox-gl-native update-submodules)
    _npm_install("${CMAKE_SOURCE_DIR}/mapbox-gl-js/test/integration" mapbox-gl-js "${CMAKE_SOURCE_DIR}/node_modules/.mapbox-gl-native.stamp")
    add_custom_target(
        npm-install ALL
        DEPENDS "${CMAKE_SOURCE_DIR}/node_modules/.mapbox-gl-js.stamp"
    )
    set_target_properties(npm-install PROPERTIES FOLDER "Misc")
endif()

# Generate source groups so the files are properly sorted in IDEs like Xcode.
function(create_source_groups target)
    get_target_property(type ${target} TYPE)
    if(type AND type STREQUAL "INTERFACE_LIBRARY")
        get_target_property(sources ${target} INTERFACE_SOURCES)
    else()
        get_target_property(sources ${target} SOURCES)
    endif()
    foreach(file ${sources})
        get_filename_component(file "${file}" ABSOLUTE)
        string(REGEX REPLACE "^${CMAKE_SOURCE_DIR}/" "" group "${file}")
        get_filename_component(group "${group}" DIRECTORY)
        string(REPLACE "/" "\\" group "${group}")
        source_group("${group}" FILES "${file}")
    endforeach()
endfunction()

function(load_sources_list VAR FILELIST)
    set(_FILES)
    file(READ "${FILELIST}" _LINES)
    # Note: this isn't really parsing JSON, but it's good enough for our purposes.
    set(_REGEX "(^|\n) *(\"([^\"]+)\" *: *)?\"([^\"]+)\",?(\n|$)")
    string(REGEX MATCHALL "${_REGEX}" _MATCHES "${_LINES}")
    foreach(_MATCH IN LISTS _MATCHES)
        string(REGEX MATCH "${_REGEX}" _FILE "${_MATCH}")
        if (_FILE AND NOT CMAKE_MATCH_3 STREQUAL "//")
            list(APPEND _FILES "${CMAKE_SOURCE_DIR}/${CMAKE_MATCH_4}")
        endif()
    endforeach()
    set(${VAR} "${_FILES}" PARENT_SCOPE)
    set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${FILELIST}")
endfunction()

function(target_sources_from_file TARGET TYPE FILELIST)
    load_sources_list(_FILELIST "${FILELIST}")
    target_sources(${TARGET} ${TYPE} "${_FILELIST}")
endfunction()

# Creates a library target for a vendored dependency
function(add_vendor_target NAME TYPE)
    set(INCLUDE_TYPE "INTERFACE")
    set(SOURCE_TYPE "INTERFACE")
    if (TYPE STREQUAL "STATIC" OR TYPE STREQUAL "SHARED")
        add_library(${NAME} ${TYPE} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/empty.cpp")
        set(INCLUDE_TYPE "PUBLIC")
        set(SOURCE_TYPE "PRIVATE")
        set_target_properties(${NAME} PROPERTIES SOURCES "")
    else()
        add_library(${NAME} ${TYPE})
    endif()
    set_target_properties(${NAME} PROPERTIES INTERFACE_SOURCES "")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/${NAME}/files.txt" FILES)
    foreach(FILE IN LISTS FILES)
        target_sources(${NAME} ${SOURCE_TYPE} "${CMAKE_CURRENT_SOURCE_DIR}/vendor/${NAME}/${FILE}")
    endforeach()
    target_include_directories(${NAME} SYSTEM ${INCLUDE_TYPE} "${CMAKE_CURRENT_SOURCE_DIR}/vendor/${NAME}/include")
    create_source_groups(${NAME})
endfunction()

# This little macro lets you set any XCode specific property
macro(set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property(TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro (set_xcode_property)

function(set_xcconfig_target_properties target)
    # Create a list of linked libraries for use in the xcconfig generation script.
    get_property(result TARGET ${target} PROPERTY INTERFACE_LINK_LIBRARIES)
    string(GENEX_STRIP "${result}" result)
    # Remove target names from the list of linker flags, since Xcode can't deal with them.
    set(link_flags)
    foreach(item IN LISTS result)
        if (NOT TARGET ${item})
            list(APPEND link_flags ${item})
        endif()
    endforeach()
    string(REGEX REPLACE "(^|;)-framework " "\\1-framework;" link_flags "${link_flags}")
    string(REPLACE ";" "\" \"" link_flags "${link_flags}")
    set_xcode_property(${target} XCCONFIG_LINK_LIBRARIES "${link_flags}")
endfunction()

# Set Xcode project build settings to be consistent with the CXX flags we're
# using. (Otherwise, Xcode's defaults may override some of these.)
function(initialize_xcode_cxx_build_settings target)
    # -Wall
    set_xcode_property(${target} GCC_WARN_SIGN_COMPARE YES)
    set_xcode_property(${target} GCC_WARN_UNINITIALIZED_AUTOS YES)
    set_xcode_property(${target} GCC_WARN_UNKNOWN_PRAGMAS YES)
    set_xcode_property(${target} GCC_WARN_UNUSED_FUNCTION YES)
    set_xcode_property(${target} GCC_WARN_UNUSED_LABEL YES)
    set_xcode_property(${target} GCC_WARN_UNUSED_PARAMETER YES)
    set_xcode_property(${target} GCC_WARN_UNUSED_VARIABLE YES)

    # -Wextra
    set_xcode_property(${target} CLANG_WARN_EMPTY_BODY YES)
    set_xcode_property(${target} GCC_WARN_ABOUT_MISSING_FIELD_INITIALIZERS YES)

    # -Wshadow
    set_xcode_property(${target} GCC_WARN_SHADOW YES)

    # -Wnon-virtual-dtor
    set_xcode_property(${target} GCC_WARN_NON_VIRTUAL_DESTRUCTOR YES)

    # -Wnon-literal-conversion
    set_xcode_property(${target} CLANG_WARN_NON_LITERAL_NULL_CONVERSION YES)

    # -Wrange-loop-analysis
    set_xcode_property(${target} CLANG_WARN_RANGE_LOOP_ANALYSIS YES)

    # -flto
    set_xcode_property(${target} LLVM_LTO $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:YES>)

    # Make all build configurations debuggable — except Release.
    set_xcode_property(${target} GCC_GENERATE_DEBUGGING_SYMBOLS $<$<NOT:$<CONFIG:Release>>:YES>)

    # -Wunguarded-availability
    set_xcode_property(${target} CLANG_WARN_UNGUARDED_AVAILABILITY YES_AGGRESSIVE)

    if (DEFINED ENV{CI})
        set_xcode_property(${target} COMPILER_INDEX_STORE_ENABLE NO)
    endif()
endfunction()

# CMake 3.1 does not have this yet.
set(CMAKE_CXX14_STANDARD_COMPILE_OPTION "-std=c++14")
set(CMAKE_CXX14_EXTENSION_COMPILE_OPTION "-std=gnu++14")
