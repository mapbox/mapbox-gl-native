if(NOT EXISTS ${PROJECT_SOURCE_DIR}/node_modules/@mapbox/cmake-node-module/module.cmake)
    message("-- Not building node bindings, dependencies not found. Run 'npm update'.")
    return()
endif()

set(NODE_MODULE_CACHE_DIR ${CMAKE_SOURCE_DIR}/build/headers)
include(${PROJECT_SOURCE_DIR}/node_modules/@mapbox/cmake-node-module/module.cmake)

add_node_module(mbgl-node
    INSTALL_PATH ${PROJECT_SOURCE_DIR}/lib/{node_abi}/mbgl.node
    NAN_VERSION 2.10.0

    # Build only for ABI 64.
    # See https://nodejs.org/en/download/releases/ for mapping of Node version to ABI number.
    EXCLUDE_NODE_ABIS 46 47 48 51 59 67 72
)

target_sources(mbgl-node INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/node_conversion.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node_expression.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node_expression.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node_feature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node_feature.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node_logging.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node_logging.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node_map.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node_map.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node_mapbox_gl_native.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node_request.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/node_request.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/util/async_queue.hpp
)

target_link_libraries(mbgl-node INTERFACE
    Mapbox::Map
)

# FIXME: Node bindings only run fully on Linux now because
# it requires libuv RunLoop (which is the default on Linux).
# Also, Sanitizer will not work here because node will do
# a dlopen(), which is not currently supported.
if(CMAKE_SYSTEM_NAME STREQUAL Linux AND NOT CMAKE_BUILD_TYPE STREQUAL Sanitize)
    add_test(NAME mbgl-node-memory COMMAND npm run test-memory WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    add_test(NAME mbgl-node-query COMMAND npm run test-query WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    add_test(NAME mbgl-node-test COMMAND npm run test WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()
