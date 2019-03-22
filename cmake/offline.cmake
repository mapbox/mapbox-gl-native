add_executable(mbgl-offline
    bin/offline.cpp
)

# args requires RTTI
# COMPILE_OPTIONS source file property was added in cmake version 3.11, so use
# the property if the cmake version is 3.11 and greater. For older version of
# cmake, use COMPILE_FLAGS to set the RTTI flag.
if(CMAKE_VERSION VERSION_LESS 3.11.0)
    set_source_files_properties(bin/offline.cpp PROPERTIES COMPILE_FLAGS "-frtti")
else()
    set_source_files_properties(bin/offline.cpp PROPERTIES COMPILE_OPTIONS "-frtti")
endif()

target_sources(mbgl-offline
    PRIVATE platform/default/include/mbgl/util/default_styles.hpp
)

target_include_directories(mbgl-offline
    PRIVATE platform/default/include
)

target_link_libraries(mbgl-offline
    PRIVATE mbgl-core
)

target_add_mason_package(mbgl-offline PRIVATE args)

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
