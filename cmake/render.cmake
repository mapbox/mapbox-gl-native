add_executable(mbgl-render
    platform/default/mbgl/renderer/async_renderer_frontend.cpp
    platform/default/mbgl/renderer/async_renderer_frontend.hpp
    bin/render.cpp
)

target_compile_options(mbgl-render
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-render
    PRIVATE platform/default
)

target_link_libraries(mbgl-render
    PRIVATE mbgl-core
)

target_add_mason_package(mbgl-render PRIVATE boost)
target_add_mason_package(mbgl-render PRIVATE boost_libprogram_options)

mbgl_platform_render()

create_source_groups(mbgl-render)

xcode_create_scheme(
    TARGET mbgl-render
    OPTIONAL_ARGS
        "--style=file.json"
        "--lon=0"
        "--lat=0"
        "--zoom=0"
        "--bearing=0"
        "--pitch=0"
        "--width=512"
        "--height=512"
        "--ratio=1"
        "--token="
        "--debug"
        "--output=out.png"
        "--cache=cache.sqlite"
        "--assets=."
)
