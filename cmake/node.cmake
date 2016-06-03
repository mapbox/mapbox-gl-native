if(NOT EXISTS ${CMAKE_SOURCE_DIR}/node_modules/node-cmake/FindNodeJS.cmake)
    message(FATAL_ERROR "Can't find node-cmake")
endif()

set(NodeJS_CXX_STANDARD 14 CACHE INTERNAL "Use C++14" FORCE)
set(NodeJS_DOWNLOAD ON CACHE INTERNAL "Download node.js sources" FORCE)
set(NodeJS_USE_CLANG_STDLIB OFF CACHE BOOL "Don't use libc++ by default" FORCE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/node_modules/node-cmake)
find_package(NodeJS)

add_nodejs_module(mbgl-node
    platform/node/src/node_mapbox_gl_native.cpp
)

target_sources(mbgl-node
    PRIVATE platform/node/src/node_log.hpp
    PRIVATE platform/node/src/node_log.cpp
    PRIVATE platform/node/src/node_map.hpp
    PRIVATE platform/node/src/node_map.cpp
    PRIVATE platform/node/src/node_request.hpp
    PRIVATE platform/node/src/node_request.cpp
    PRIVATE platform/node/src/node_feature.hpp
    PRIVATE platform/node/src/node_feature.cpp
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
)

target_link_libraries(mbgl-node
    PRIVATE mbgl-core
)

add_custom_command(
    TARGET mbgl-node
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mbgl-node> ${CMAKE_SOURCE_DIR}/lib/mapbox-gl-native.node
)

mbgl_platform_node()
