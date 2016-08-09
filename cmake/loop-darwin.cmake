add_library(mbgl-loop STATIC
    platform/darwin/src/async_task.cpp
    platform/darwin/src/run_loop.cpp
    platform/darwin/src/timer.cpp
)

target_compile_options(mbgl-loop
    PRIVATE -fPIC
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-loop
    PUBLIC include
    PRIVATE src
)

create_source_groups(mbgl-loop)
target_append_xcconfig(mbgl-loop)
