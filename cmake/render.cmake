add_executable(mbgl-render
    bin/render.cpp
)

target_include_directories(mbgl-render
    PRIVATE platform/default/include
)

target_link_libraries(mbgl-render
    PRIVATE mbgl-core
    PRIVATE Mapbox::Base::Extras::args
)

mbgl_platform_render()

create_source_groups(mbgl-render)

initialize_xcode_cxx_build_settings(mbgl-render)

set_target_properties(mbgl-render PROPERTIES FOLDER "Executables")

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
