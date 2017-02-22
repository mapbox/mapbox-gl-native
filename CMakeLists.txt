cmake_minimum_required(VERSION 3.1)
project(mbgl LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 14)

include(cmake/mbgl.cmake)
include(cmake/mason.cmake)

option(WITH_CXX11ABI "Use cxx11abi mason packages" OFF)
option(WITH_COVERAGE "Enable coverage reports" OFF)
option(WITH_OSMESA   "Use OSMesa headless backend" OFF)
option(WITH_EGL      "Use EGL backend" OFF)

if(WITH_CXX11ABI)
    set(MASON_CXXABI_SUFFIX -cxx11abi)
endif()

if(WITH_OSMESA AND WITH_EGL)
    message(FATAL_ERROR "WITH_OSMESA and WITH_EGL are mutually exclusive.")
endif()

if(WITH_EGL)
    add_definitions(-DMBGL_USE_GLES2=1)
endif()

if($ENV{CI})
    add_compile_options(-DCI_BUILD=1)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/.git/HEAD)
    exec_program(
        "git"
        ${CMAKE_SOURCE_DIR}
        ARGS "rev-parse --short=8 HEAD"
        OUTPUT_VARIABLE MBGL_VERSION_REV )
else()
    set(MBGL_VERSION_REV 00000000)
endif()

set_source_files_properties(src/mbgl/util/version.cpp PROPERTIES COMPILE_DEFINITIONS MBGL_VERSION_REV="${MBGL_VERSION_REV}")

mason_use(geometry VERSION 0.9.0 HEADER_ONLY)
mason_use(variant VERSION 1.1.4 HEADER_ONLY)
mason_use(unique_resource VERSION cba309e HEADER_ONLY)
mason_use(rapidjson VERSION 1.1.0 HEADER_ONLY)
mason_use(boost VERSION 1.62.0 HEADER_ONLY)
mason_use(geojsonvt VERSION 6.2.0 HEADER_ONLY)
mason_use(supercluster VERSION 0.2.0-1 HEADER_ONLY)
mason_use(kdbush VERSION 0.1.1-1 HEADER_ONLY)
mason_use(earcut VERSION 0.12.1 HEADER_ONLY)
mason_use(protozero VERSION 1.4.2 HEADER_ONLY)
mason_use(pixelmatch VERSION 0.10.0 HEADER_ONLY)
mason_use(geojson VERSION 0.4.0 HEADER_ONLY)
mason_use(polylabel VERSION 1.0.2 HEADER_ONLY)
mason_use(wagyu VERSION 0.4.1 HEADER_ONLY)

add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)

if(WITH_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif(WITH_COVERAGE)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -ftemplate-depth=1024 -Wall -Wextra -Wshadow -Werror -Wno-variadic-macros -Wno-unknown-pragmas")
if(APPLE)
    # -Wno-error=unused-command-line-argument is required due to https://llvm.org/bugs/show_bug.cgi?id=7798
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-command-line-argument")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG")

if(CMAKE_COMPILER_IS_GNUCXX)
    # https://svn.boost.org/trac/boost/ticket/9240
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")
endif()

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/platform/${MBGL_PLATFORM}/config.cmake)
    message(ERROR "Can't find config.cmake file for platform ${MBGL_PLATFORM}")
endif()

include(cmake/loop-uv.cmake)

include(platform/${MBGL_PLATFORM}/config.cmake)

include(cmake/core-files.cmake)
include(cmake/core.cmake)

if(COMMAND mbgl_platform_test)
    include(cmake/test-files.cmake)
    include(cmake/test.cmake)
endif()

if(COMMAND mbgl_platform_benchmark)
    include(cmake/benchmark-files.cmake)
    include(cmake/benchmark.cmake)
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
