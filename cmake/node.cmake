# Load Node.js
set(NODE_MODULE_CACHE_DIR "${CMAKE_SOURCE_DIR}/build/headers")
include(node_modules/@mapbox/cmake-node-module/module.cmake)

add_library(mbgl-loop-node INTERFACE)

target_sources(mbgl-loop-node INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/default/async_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/default/run_loop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/default/timer.cpp
)

target_include_directories(mbgl-loop-node INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

create_source_groups(mbgl-loop-node)


add_node_module(mbgl-node
    INSTALL_PATH "lib/{node_abi}/mbgl.node"
    NAN_VERSION "2.10.0"
    
    # Don't build for Node 4.x, 5.x, 7.x, or 9.x
    # See https://nodejs.org/en/download/releases/ for mapping of Node version to ABI number.
    # Changes to this property should happen in tandem with updates to the version targets
    # in platform/node/scripts/publish.sh and the "node" engines property in package.json.
    EXCLUDE_NODE_ABIS 46 47 51 59
)

target_sources(mbgl-node INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_mapbox_gl_native.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_mapbox_gl_native.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_logging.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_logging.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_conversion.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_map.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_request.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_request.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_feature.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_feature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_thread_pool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_thread_pool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_expression.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/node_expression.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/node/src/util/async_queue.hpp
)

target_include_directories(mbgl-node INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/default
)

target_link_libraries(mbgl-node INTERFACE
    mbgl-core
    mbgl-loop-node
)

target_add_mason_package(mbgl-node INTERFACE geojson)
set_target_properties(mbgl-node.all PROPERTIES FOLDER "Node.js")

add_custom_target(mbgl-node.active DEPENDS mbgl-node.abi-${NodeJS_ABI})
set_target_properties(mbgl-node.active PROPERTIES FOLDER "Node.js")

mbgl_platform_node()

create_source_groups(mbgl-node)

foreach(ABI IN LISTS mbgl-node::abis)
    initialize_xcode_cxx_build_settings(mbgl-node.abi-${ABI})
    set_target_properties(mbgl-node.abi-${ABI} PROPERTIES FOLDER "Node.js")
    xcode_create_scheme(
        TARGET mbgl-node.abi-${ABI}
        NAME "mbgl-node (ABI ${ABI})"
    )
endforeach()

xcode_create_scheme(
    TARGET mbgl-node.active
    TYPE library
    NAME "mbgl-node (Active ABI)"
)

xcode_create_scheme(
    TARGET mbgl-node.all
    TYPE library
    NAME "mbgl-node (All ABIs)"
)

xcode_create_scheme(
    TARGET mbgl-node.active
    TYPE node
    NAME "node tests (Active ABI)"
    ARGS
        "node_modules/.bin/tape platform/node/test/js/**/*.test.js"
)

xcode_create_scheme(
    TARGET mbgl-node.active
    TYPE node
    NAME "node render tests (Active ABI)"
    ARGS
        "-r esm"
        "platform/node/test/render.test.js"
    OPTIONAL_ARGS
        "group"
        "test"
)

xcode_create_scheme(
    TARGET mbgl-node.active
    TYPE node
    NAME "node query tests (Active ABI)"
    ARGS
        "-r esm"
        "platform/node/test/query.test.js"
    OPTIONAL_ARGS
        "group"
        "test"
)

xcode_create_scheme(
    TARGET mbgl-node.active
    TYPE node
    NAME "node expression tests (Active ABI)"
    ARGS
        "-r esm"
        "platform/node/test/expression.test.js"
    OPTIONAL_ARGS
        "group"
        "test"
)

xcode_create_scheme(
    TARGET mbgl-node.active
    TYPE node
    NAME "node-benchmark (Active ABI)"
    ARGS
        "-r esm"
        "platform/node/test/benchmark.js"
    OPTIONAL_ARGS
        "group"
        "test"
)
