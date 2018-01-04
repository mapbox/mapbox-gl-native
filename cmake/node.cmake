# Load Node.js
include(cmake/NodeJS.cmake)
nodejs_init()

add_nodejs_module(mbgl-node
    platform/node/src/node_mapbox_gl_native.cpp
)

# NodeJS.cmake forces C++11.
# https://github.com/cjntaylor/node-cmake/issues/18
set_target_properties("mbgl-node" PROPERTIES CXX_STANDARD 14)

target_sources(mbgl-node
    PRIVATE platform/node/src/node_logging.hpp
    PRIVATE platform/node/src/node_logging.cpp
    PRIVATE platform/node/src/node_conversion.hpp
    PRIVATE platform/node/src/node_map.hpp
    PRIVATE platform/node/src/node_map.cpp
    PRIVATE platform/node/src/node_request.hpp
    PRIVATE platform/node/src/node_request.cpp
    PRIVATE platform/node/src/node_feature.hpp
    PRIVATE platform/node/src/node_feature.cpp
    PRIVATE platform/node/src/node_thread_pool.hpp
    PRIVATE platform/node/src/node_thread_pool.cpp
    PRIVATE platform/node/src/node_expression.hpp
    PRIVATE platform/node/src/node_expression.cpp
    PRIVATE platform/node/src/util/async_queue.hpp
)

target_include_directories(mbgl-node
    PRIVATE platform/default
)

# Use node-provided uv.h. This is not part of loop-uv.cmake because loop-uv.cmake is also
# used by linux/config.cmake, where we need to use headers provided by mason's libuv.
target_include_directories(mbgl-loop-uv PUBLIC ${NODEJS_INCLUDE_DIRS})

target_link_libraries(mbgl-node
    PRIVATE mbgl-core
    PRIVATE mbgl-loop-uv
)

target_add_mason_package(mbgl-node PRIVATE geojson)

add_custom_command(
    TARGET mbgl-node
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mbgl-node> ${CMAKE_SOURCE_DIR}/lib/mapbox_gl_native.node
)

mbgl_platform_node()

create_source_groups(mbgl-node)

initialize_xcode_cxx_build_settings(mbgl-node)

xcode_create_scheme(
    TARGET mbgl-node
)

xcode_create_scheme(
    TARGET mbgl-node
    TYPE node
    NAME "node tests"
    ARGS
        "node_modules/.bin/tape platform/node/test/js/**/*.test.js"
)

xcode_create_scheme(
    TARGET mbgl-node
    TYPE node
    NAME "node render tests"
    ARGS
        "platform/node/test/render.test.js"
    OPTIONAL_ARGS
        "group"
        "test"
)

xcode_create_scheme(
    TARGET mbgl-node
    TYPE node
    NAME "node query tests"
    ARGS
        "platform/node/test/query.test.js"
    OPTIONAL_ARGS
        "group"
        "test"
)

xcode_create_scheme(
    TARGET mbgl-node
    TYPE node
    NAME "node expression tests"
    ARGS
        "platform/node/test/expression.test.js"
    OPTIONAL_ARGS
        "group"
        "test"
)

xcode_create_scheme(
    TARGET mbgl-node
    TYPE node
    NAME "node-benchmark"
    ARGS
        "platform/node/test/benchmark.js"
    OPTIONAL_ARGS
        "group"
        "test"
)
