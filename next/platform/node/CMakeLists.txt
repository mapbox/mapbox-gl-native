if(NOT EXISTS ${MBGL_ROOT}/node_modules/@mapbox/cmake-node-module/module.cmake)
    message("-- Not building node bindings, dependencies not found. Run 'npm update'.")
    return()
endif()

set(NODE_MODULE_CACHE_DIR ${CMAKE_SOURCE_DIR}/build/headers)
include(${MBGL_ROOT}/node_modules/@mapbox/cmake-node-module/module.cmake)

add_node_module(
    mbgl-node
    INSTALL_PATH ${MBGL_ROOT}/lib/{node_abi}/mbgl.node
    NAN_VERSION 2.10.0
    EXCLUDE_NODE_ABIS
        46
        47
        48
        51
        59
        67
        72
        79
)

target_sources(
    mbgl-node
    INTERFACE
        ${MBGL_ROOT}/platform/node/src/node_conversion.hpp
        ${MBGL_ROOT}/platform/node/src/node_expression.cpp
        ${MBGL_ROOT}/platform/node/src/node_expression.hpp
        ${MBGL_ROOT}/platform/node/src/node_feature.cpp
        ${MBGL_ROOT}/platform/node/src/node_feature.hpp
        ${MBGL_ROOT}/platform/node/src/node_logging.cpp
        ${MBGL_ROOT}/platform/node/src/node_logging.hpp
        ${MBGL_ROOT}/platform/node/src/node_map.cpp
        ${MBGL_ROOT}/platform/node/src/node_map.hpp
        ${MBGL_ROOT}/platform/node/src/node_mapbox_gl_native.cpp
        ${MBGL_ROOT}/platform/node/src/node_request.cpp
        ${MBGL_ROOT}/platform/node/src/node_request.hpp
        ${MBGL_ROOT}/platform/node/src/util/async_queue.hpp
)

target_link_libraries(
    mbgl-node
    INTERFACE Mapbox::Map
)

# FIXME: Node bindings only run fully on Linux now because it requires libuv RunLoop (which is the default on Linux). Also, Sanitizer will
# not work here because node will do a dlopen(), which is not currently supported.
if(CMAKE_SYSTEM_NAME STREQUAL Linux AND NOT CMAKE_BUILD_TYPE STREQUAL Sanitize)
    add_test(NAME mbgl-node-memory COMMAND npm run test-memory WORKING_DIRECTORY ${MBGL_ROOT})
    add_test(NAME mbgl-node-query COMMAND npm run test-query WORKING_DIRECTORY ${MBGL_ROOT})
    add_test(NAME mbgl-node-test COMMAND npm run test WORKING_DIRECTORY ${MBGL_ROOT})
endif()
