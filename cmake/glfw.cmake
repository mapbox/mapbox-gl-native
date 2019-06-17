add_executable(mbgl-glfw
    platform/glfw/main.cpp
)

target_sources(mbgl-glfw
    PRIVATE platform/glfw/glfw_view.hpp
    PRIVATE platform/glfw/glfw_view.cpp
    PRIVATE platform/glfw/glfw_backend.hpp
    PRIVATE platform/glfw/glfw_gl_backend.hpp
    PRIVATE platform/glfw/glfw_gl_backend.cpp
    PRIVATE platform/glfw/glfw_renderer_frontend.hpp
    PRIVATE platform/glfw/glfw_renderer_frontend.cpp
    PRIVATE platform/glfw/settings_json.hpp
    PRIVATE platform/glfw/settings_json.cpp
    PRIVATE platform/default/include/mbgl/util/default_styles.hpp
)

target_include_directories(mbgl-glfw
    PRIVATE platform/glfw
    PRIVATE platform/default/include
)

target_link_libraries(mbgl-glfw
    PRIVATE mbgl-core
    PRIVATE glfw
    PRIVATE cheap-ruler-cpp
    PRIVATE args
)

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
