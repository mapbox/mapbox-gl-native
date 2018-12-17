set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)

unset(CMAKE_C_VISIBILITY_PRESET)
add_subdirectory(vendor/glfw)
set(CMAKE_C_VISIBILITY_PRESET hidden)

target_compile_options(glfw PRIVATE -Wno-shadow -Wno-unused-parameter -Wno-sign-compare -Wno-missing-field-initializers)

if(APPLE)
    # Don't show OpenGL deprecation warnings.
    target_compile_options(glfw PRIVATE -Wno-deprecated-declarations -Wno-unguarded-availability-new)

    # get_target_property(DEPLOYMENT_TARGET_VERSION glfw MACOSX_DEPLOYMENT_TARGET)
    # message(STATUS "target: ${DEPLOYMENT_TARGET_VERSION}")
    set_target_properties(glfw PROPERTIES OSX_DEPLOYMENT_TARGET 10.11)
endif()

if(CMAKE_GENERATOR STREQUAL "Xcode")
    # The Xcode generator always tries to link from this directory instead of the actual output dir.
    set_target_properties(glfw PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
endif()
