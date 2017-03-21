add_executable(mbgl-render
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
