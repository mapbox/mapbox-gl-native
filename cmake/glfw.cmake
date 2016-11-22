add_executable(mbgl-glfw
    bin/glfw.cpp
)

target_sources(mbgl-glfw
    PRIVATE include/mbgl/platform/default/glfw_view.hpp
    PRIVATE platform/default/glfw_view.cpp
    PRIVATE include/mbgl/platform/default/settings_json.hpp
    PRIVATE platform/default/settings_json.cpp
    PRIVATE platform/default/mbgl/util/default_styles.hpp
    PRIVATE platform/default/mbgl/util/default_styles.cpp
)

target_compile_options(mbgl-glfw
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-glfw
    PRIVATE include
    PRIVATE src # TODO: eliminate
    PRIVATE platform/default
)

target_link_libraries(mbgl-glfw
    PRIVATE mbgl-core
)

add_custom_command(
    TARGET mbgl-glfw POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/common/ca-bundle.crt
            ${CMAKE_CURRENT_BINARY_DIR}/ca-bundle.crt
)

target_add_mason_package(mbgl-glfw PRIVATE glfw)

mbgl_platform_glfw()

create_source_groups(mbgl-glfw)
