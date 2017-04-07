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
    PRIVATE platform/node/src/node_map.hpp
    PRIVATE platform/node/src/node_map.cpp
    PRIVATE platform/node/src/node_request.hpp
    PRIVATE platform/node/src/node_request.cpp
    PRIVATE platform/node/src/node_feature.hpp
    PRIVATE platform/node/src/node_feature.cpp
    PRIVATE platform/node/src/node_thread_pool.hpp
    PRIVATE platform/node/src/node_thread_pool.cpp
    PRIVATE platform/node/src/util/async_queue.hpp
)

target_compile_options(mbgl-node
    PRIVATE -fPIC
    PRIVATE -fvisibility-inlines-hidden
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

set_target_properties(mbgl-node PROPERTIES
    OUTPUT_NAME "mapbox_gl_native"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
)

mbgl_platform_node()

if(WITH_SWIFTSHADER)
    mbgl_platform_set_swiftshader_rpath(mbgl-node)
endif()

create_source_groups(mbgl-node)
