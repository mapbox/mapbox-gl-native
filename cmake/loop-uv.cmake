add_library(mbgl-loop-uv INTERFACE)

target_sources(mbgl-loop-uv INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/default/src/mbgl/util/async_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/default/src/mbgl/util/run_loop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/default/src/mbgl/util/timer.cpp
)

target_include_directories(mbgl-loop-uv INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

create_source_groups(mbgl-loop-uv)
