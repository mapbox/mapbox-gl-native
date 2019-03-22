add_executable(mbgl-render
    bin/render.cpp
)

# args requires RTTI
# COMPILE_OPTIONS source file property was added in cmake version 3.11, so use
# the property if the cmake version is 3.11 and greater. For older version of
# cmake, use COMPILE_FLAGS to set the RTTI flag.
if(CMAKE_VERSION VERSION_LESS 3.11.0)
    set_source_files_properties(bin/render.cpp PROPERTIES COMPILE_FLAGS "-frtti")
else()
    set_source_files_properties(bin/render.cpp PROPERTIES COMPILE_OPTIONS "-frtti")
endif()

target_include_directories(mbgl-render
    PRIVATE platform/default/include
)

target_link_libraries(mbgl-render
    PRIVATE mbgl-core
)

target_add_mason_package(mbgl-render PRIVATE args)

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
