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

    function(_npm_install DIRECTORY NAME ADDITIONAL_DEPS)
        SET(NPM_INSTALL_FAILED FALSE)
        if("${DIRECTORY}/package.json" IS_NEWER_THAN "${DIRECTORY}/node_modules/.${NAME}.stamp")
            message(STATUS "Running 'npm install' for ${NAME}...")
            execute_process(
                COMMAND ${NodeJS_EXECUTABLE} ${npm_EXECUTABLE} install --ignore-scripts
                WORKING_DIRECTORY "${DIRECTORY}"
                RESULT_VARIABLE NPM_INSTALL_FAILED)
            if(NOT NPM_INSTALL_FAILED)
                execute_process(COMMAND ${CMAKE_COMMAND} -E touch "${DIRECTORY}/node_modules/.${NAME}.stamp")
            endif()
        endif()

        add_custom_command(
            OUTPUT "${DIRECTORY}/node_modules/.${NAME}.stamp"
            COMMAND ${NodeJS_EXECUTABLE} ${npm_EXECUTABLE} install --ignore-scripts
            COMMAND ${CMAKE_COMMAND} -E touch "${DIRECTORY}/node_modules/.${NAME}.stamp"
            WORKING_DIRECTORY "${DIRECTORY}"
            DEPENDS ${ADDITIONAL_DEPS} "${DIRECTORY}/package.json"
            COMMENT "Running 'npm install' for ${NAME}...")
    endfunction()

    # Run submodule update
    message(STATUS "Updating submodules...")
    execute_process(
        COMMAND git submodule update --init mapbox-gl-js
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
        COMMAND git submodule update --init mapbox-gl-js
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Updating submodules..."
    )

    # Run npm install for both directories, and add custom commands, and a target that depends on them.
    _npm_install("${CMAKE_SOURCE_DIR}" mapbox-gl-native update-submodules)
    _npm_install("${CMAKE_SOURCE_DIR}/mapbox-gl-js/test/integration" mapbox-gl-js "${CMAKE_SOURCE_DIR}/node_modules/.mapbox-gl-native.stamp")
    add_custom_target(
        npm-install ALL
        DEPENDS "${CMAKE_SOURCE_DIR}/node_modules/.mapbox-gl-js.stamp"
    )
endif()

# Generate source groups so the files are properly sorted in IDEs like Xcode.
function(create_source_groups target)
    get_target_property(sources ${target} SOURCES)
    foreach(file ${sources})
        get_filename_component(file "${file}" ABSOLUTE)
        string(REGEX REPLACE "^${CMAKE_SOURCE_DIR}/" "" group "${file}")
        get_filename_component(group "${group}" DIRECTORY)
        string(REPLACE "/" "\\" group "${group}")
        source_group("${group}" FILES "${file}")
    endforeach()
endfunction()

# This little macro lets you set any XCode specific property
macro(set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property(TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro (set_xcode_property)

function(_get_xcconfig_property target var)
    get_property(result TARGET ${target} PROPERTY INTERFACE_${var} SET)
    if (result)
        get_property(result TARGET ${target} PROPERTY INTERFACE_${var})
        if (var STREQUAL "LINK_LIBRARIES")
            # Remove target names from the list of linker flags, since Xcode can't deal with them.
            set(link_flags)
            foreach(item IN LISTS result)
                if (NOT TARGET ${item})
                    list(APPEND link_flags ${item})
                endif()
            endforeach()
            set(result "${link_flags}")
        endif()
        string(REPLACE ";-framework " ";-framework;" result "${result}")
        string(REPLACE ";" "\" \"" result "${result}")
        string(REPLACE "-" "_" target "${target}")
        set(${target}_${var} "${result}" PARENT_SCOPE)
    endif()
endfunction()

if(MBGL_PLATFORM STREQUAL "ios")
    execute_process(
        COMMAND git submodule update --init platform/ios/vendor/SMCalloutView platform/ios/uitest/KIF platform/ios/uitest/OHHTTPStubs
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

function(write_xcconfig_target_properties)
    foreach(target ${ARGN})
        _get_xcconfig_property(${target} INCLUDE_DIRECTORIES)
        _get_xcconfig_property(${target} LINK_LIBRARIES)
    endforeach()
    configure_file(
        "${CMAKE_SOURCE_DIR}/scripts/config.xcconfig.in"
        "${CMAKE_BINARY_DIR}/config.xcconfig"
        @ONLY
    )
endfunction()

# Set Xcode project build settings to be consistent with the CXX flags we're
# using. (Otherwise, Xcode's defaults may override some of these.)
macro(initialize_xcode_cxx_build_settings target)
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
endmacro(initialize_xcode_cxx_build_settings)

# CMake 3.1 does not have this yet.
set(CMAKE_CXX14_STANDARD_COMPILE_OPTION "-std=c++14")
set(CMAKE_CXX14_EXTENSION_COMPILE_OPTION "-std=gnu++14")
