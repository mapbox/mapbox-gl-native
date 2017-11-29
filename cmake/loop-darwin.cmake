add_library(mbgl-loop-darwin STATIC
    platform/darwin/src/async_task.cpp
    platform/darwin/src/run_loop.cpp
    platform/darwin/src/timer.cpp
)

target_include_directories(mbgl-loop-darwin
    PRIVATE include
    PRIVATE src
)

create_source_groups(mbgl-loop-darwin)

xcode_create_scheme(TARGET mbgl-loop-darwin)