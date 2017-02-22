add_library(mbgl-loop-uv STATIC
    platform/default/async_task.cpp
    platform/default/run_loop.cpp
    platform/default/timer.cpp
)

target_compile_options(mbgl-loop-uv
    PRIVATE -fPIC
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-loop-uv
    PUBLIC include
    PRIVATE src
)

create_source_groups(mbgl-loop-uv)
target_append_xcconfig(mbgl-loop-uv)
