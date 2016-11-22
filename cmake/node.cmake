add_nodejs_module(mbgl-node
    platform/node/src/node_mapbox_gl_native.cpp
)

target_sources(mbgl-node
    PRIVATE platform/node/src/node_logging.hpp
    PRIVATE platform/node/src/node_logging.cpp
    PRIVATE platform/node/src/node_map.hpp
    PRIVATE platform/node/src/node_map.cpp
    PRIVATE platform/node/src/node_request.hpp
    PRIVATE platform/node/src/node_request.cpp
    PRIVATE platform/node/src/node_feature.hpp
    PRIVATE platform/node/src/node_feature.cpp
    PRIVATE platform/node/src/node_thread_pool.hpp
    PRIVATE platform/node/src/node_thread_pool.cpp
    PRIVATE platform/node/src/util/async_queue.hpp

    # We are compiling with the uv loop, but since this target already has the headers for libuv,
    # we don't have to install it manually.
    PRIVATE platform/default/async_task.cpp
    PRIVATE platform/default/run_loop.cpp
    PRIVATE platform/default/timer.cpp
)

target_compile_options(mbgl-node
    PRIVATE -fPIC
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-node
    PRIVATE src # TODO: eliminate
    PRIVATE platform/default
)

target_link_libraries(mbgl-node
    PRIVATE mbgl-core
)

target_add_mason_package(mbgl-node PRIVATE geojson)

add_custom_command(
    TARGET mbgl-node
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mbgl-node> ${CMAKE_SOURCE_DIR}/lib/mapbox-gl-native.node
)

mbgl_platform_node()

create_source_groups(mbgl-node)
