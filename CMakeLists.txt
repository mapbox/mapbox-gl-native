cmake_minimum_required(VERSION 3.1)
project(mbgl LANGUAGES CXX C)

include(cmake/mbgl.cmake)

if(NOT EXISTS ".mason/mason.cmake")
   execute_process(
        COMMAND git submodule update --init .mason
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

include(.mason/mason.cmake)

include(cmake/npm.cmake)

option(WITH_CXX11ABI "Use cxx11abi mason packages" OFF)
option(WITH_COVERAGE "Enable coverage reports" OFF)

if(WITH_CXX11ABI)
    set(MASON_CXXABI_SUFFIX -cxx11abi)
endif()

mason_use(geometry VERSION 0.8.0 HEADER_ONLY)
mason_use(variant VERSION 1.1.0 HEADER_ONLY)
mason_use(unique_resource VERSION dev HEADER_ONLY)
mason_use(rapidjson VERSION 1.0.2 HEADER_ONLY)
mason_use(boost VERSION 1.60.0 HEADER_ONLY)
mason_use(geojsonvt VERSION 6.1.2 HEADER_ONLY)
mason_use(supercluster VERSION 0.2.0 HEADER_ONLY)
mason_use(kdbush VERSION 0.1.1 HEADER_ONLY)
mason_use(earcut VERSION 0.11 HEADER_ONLY)
mason_use(protozero VERSION 1.3.0 HEADER_ONLY)
mason_use(pixelmatch VERSION 0.9.0 HEADER_ONLY)
mason_use(geojson VERSION 0.1.4${MASON_CXXABI_SUFFIX})

if(WITH_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif(WITH_COVERAGE)

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
