add_executable(mbgl-glfw
    platform/glfw/main.cpp
)

# args requires RTTI
# COMPILE_OPTIONS source file property was added in cmake version 3.11, so use
# the property if the cmake version is 3.11 and greater. For older version of
# cmake, use COMPILE_FLAGS to set the RTTI flag.
if(CMAKE_VERSION VERSION_LESS 3.11.0)
    set_source_files_properties(platform/glfw/main.cpp PROPERTIES COMPILE_FLAGS "-frtti")
else()
    set_source_files_properties(platform/glfw/main.cpp PROPERTIES COMPILE_OPTIONS "-frtti")
endif()

target_sources(mbgl-glfw
    PRIVATE platform/glfw/glfw_view.hpp
    PRIVATE platform/glfw/glfw_view.cpp
    PRIVATE platform/glfw/glfw_renderer_frontend.hpp
    PRIVATE platform/glfw/glfw_renderer_frontend.cpp
    PRIVATE platform/glfw/settings_json.hpp
    PRIVATE platform/glfw/settings_json.cpp
    PRIVATE platform/default/include/mbgl/util/default_styles.hpp
)

target_include_directories(mbgl-glfw
    PRIVATE platform/default/include
)

target_link_libraries(mbgl-glfw
    PRIVATE mbgl-core
    PRIVATE glfw
    PRIVATE cheap-ruler-cpp
)

target_add_mason_package(mbgl-glfw PRIVATE args)

mbgl_platform_glfw()

create_source_groups(mbgl-glfw)

set_target_properties(mbgl-glfw PROPERTIES FOLDER "Executables")

initialize_xcode_cxx_build_settings(mbgl-glfw)

xcode_create_scheme(
    TARGET mbgl-glfw
    OPTIONAL_ARGS
        "--style=file.json"
        "--cache=/tmp/mbgl-cache.db"
        "--lon=0"
        "--lat=0"
        "--zoom=1"
        "--bearing=0"
        "--pitch=0"
        "--fullscreen"
        "--offline"
        "--benchmark"
)
