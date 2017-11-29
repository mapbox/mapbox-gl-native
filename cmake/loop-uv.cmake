add_library(mbgl-loop-uv STATIC
    platform/default/async_task.cpp
    platform/default/run_loop.cpp
    platform/default/timer.cpp
)

target_include_directories(mbgl-loop-uv
    PRIVATE include
    PRIVATE src
)

target_link_libraries(mbgl-loop-uv
    PRIVATE mbgl-core
)

create_source_groups(mbgl-loop-uv)

xcode_create_scheme(TARGET mbgl-loop-uv)