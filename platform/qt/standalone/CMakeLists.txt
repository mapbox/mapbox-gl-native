cmake_minimum_required(VERSION 3.2)
project(qmapboxgl LANGUAGES CXX C)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../..)

include(${CMAKE_SOURCE_DIR}/cmake/core-files.cmake)
include(${CMAKE_SOURCE_DIR}/platform/qt/qt.cmake)
include(${CMAKE_SOURCE_DIR}/platform/qt/standalone/standalone.cmake)

add_library(mbgl-core STATIC
    ${MBGL_CORE_FILES}
)

add_dep(geometry.hpp      v0.9.0  mapbox/geometry.hpp)
add_dep(variant           v1.1.4  mapbox/variant)
add_dep(unique_resource   cba309  okdshin/unique_resource)
add_dep(rapidjson         v1.1.0  miloyip/rapidjson)
add_dep(libboost          v1.60.0 tmpsantos/mapbox-gl-native-boost)
add_dep(geojson-vt-cpp    v6.1.3  mapbox/geojson-vt-cpp)
add_dep(supercluster.hpp  v0.2.0  mapbox/supercluster.hpp)
add_dep(kdbush.hpp        v0.1.1  mourner/kdbush.hpp)
add_dep(earcut            v0.12.1 mapbox/earcut.hpp)
add_dep(protozero         v1.4.2  mapbox/protozero)
add_dep(geojson-cpp       v0.4.0  mapbox/geojson-cpp)
add_dep(polylabel         20daf8  mapbox/polylabel) # v.1.0.2 + include fix
add_dep(mapbox-gl-shaders 878008  mapbox/mapbox-gl-js)
add_dep(optional          5f08e2  tmpsantos/Optional) # polyfill for Windows

add_shader(circle)
add_shader(collision_box)
add_shader(debug)
add_shader(fill)
add_shader(fill_outline)
add_shader(fill_outline_pattern)
add_shader(fill_pattern)
add_shader(line)
add_shader(line_pattern)
add_shader(line_sdf)
add_shader(raster)
add_shader(symbol_icon)
add_shader(symbol_sdf)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    add_definitions(
        -I/usr/local/opt/icu4c/include
    )
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(
        -D_USE_MATH_DEFINES
        -DNOGDI
        -DNOMINMAX
    )
endif()

target_sources(mbgl-core
    ${MBGL_QT_FILES}
    PRIVATE ${CMAKE_SOURCE_DIR}/platform/qt/src/bidi.cpp
    PRIVATE ${CMAKE_SOURCE_DIR}/platform/qt/src/utf.cpp
)

target_link_libraries(mbgl-core
    ${MBGL_QT_LIBRARIES}
)

add_custom_target(mbgl-headers DEPENDS
    ${MBGL_SHADER_FILES}
    ${CMAKE_SOURCE_DIR}/include/mbgl/util/version.hpp
)

add_dependencies(mbgl-core
    mbgl-headers
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/platform/default
    ${CMAKE_SOURCE_DIR}/platform/qt/include
    ${CMAKE_SOURCE_DIR}/src
)

add_definitions(
    -DQT_IMAGE_DECODERS
    -DRAPIDJSON_HAS_STDSTRING=1
)
