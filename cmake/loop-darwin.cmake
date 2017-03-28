add_library(mbgl-loop-darwin STATIC
    platform/darwin/src/async_task.cpp
    platform/darwin/src/run_loop.cpp
    platform/darwin/src/timer.cpp
)

set_xcode_property(mbgl-loop-darwin GCC_SYMBOLS_PRIVATE_EXTERN YES)

target_compile_options(mbgl-loop-darwin
    PRIVATE -fPIC
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-loop-darwin
    PRIVATE include
    PRIVATE src
)

create_source_groups(mbgl-loop-darwin)
