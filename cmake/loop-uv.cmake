add_library(mbgl-loop STATIC
    platform/default/async_task.cpp
    platform/default/run_loop.cpp
    platform/default/timer.cpp
)

target_compile_options(mbgl-loop
    PRIVATE -fPIC
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-loop
    PUBLIC include
    PRIVATE src
)

target_add_mason_package(mbgl-loop PUBLIC libuv)

create_source_groups(mbgl-loop)
