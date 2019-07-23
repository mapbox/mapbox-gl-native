add_executable(mbgl-offline
    bin/offline.cpp
)

target_include_directories(mbgl-offline
    PRIVATE platform/default/include
)

target_link_libraries(mbgl-offline
    PRIVATE mbgl-core
    PRIVATE Mapbox::Base::Extras::args
)

mbgl_platform_offline()

create_source_groups(mbgl-offline)

initialize_xcode_cxx_build_settings(mbgl-offline)

set_target_properties(mbgl-offline PROPERTIES FOLDER "Executables")

xcode_create_scheme(
    TARGET mbgl-offline
    OPTIONAL_ARGS
        "--style=file.json"
        "--north=37.2"
        "--west=-122.8"
        "--south=38.1"
        "--east=-121.7"
        "--minZoom=0.0"
        "--maxZoom=15.0"
        "--pixelRatio=1.0"
        "--token="
        "--output=offline.db"
)
