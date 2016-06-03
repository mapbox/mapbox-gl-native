cmake_minimum_required(VERSION 3.2)
project(mbgl LANGUAGES CXX C)
include(cmake/npm.cmake)
include(cmake/mason.cmake)
include(cmake/mbgl.cmake)

mason_use(geometry 0.8.0)
mason_use(variant 1.1.0)
mason_use(unique_resource dev)
mason_use(rapidjson 1.0.2)
mason_use(boost 1.60.0)
mason_use(geojson 0.1.4)
mason_use(geojsonvt 6.1.2)
mason_use(supercluster 0.2.0)
mason_use(kdbush 0.1.1)
mason_use(earcut 0.11)
mason_use(protozero 1.3.0)
mason_use(gtest 1.7.0)
mason_use(pixelmatch 0.9.0)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wshadow -Wno-variadic-macros -Wno-unknown-pragmas")

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/platform/${MBGL_PLATFORM}/config.cmake)
    message(ERROR "Can't find config.cmake file for platform ${MBGL_PLATFORM}")
endif()

include(platform/${MBGL_PLATFORM}/config.cmake)

include(cmake/core-files.cmake)
include(cmake/shaders.cmake)
include(cmake/core.cmake)

if(COMMAND mbgl_platform_test)
    include(cmake/test-files.cmake)
    include(cmake/test.cmake)
endif()

if(COMMAND mbgl_platform_glfw)
    include(cmake/glfw.cmake)
endif()

if(COMMAND mbgl_platform_render)
    include(cmake/render.cmake)
endif()

if(COMMAND mbgl_platform_offline)
    include(cmake/offline.cmake)
endif()

if(COMMAND mbgl_platform_node)
    include(cmake/node.cmake)
endif()
