execute_process(
    COMMAND git submodule update --init .
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vendor
)

include(${CMAKE_SOURCE_DIR}/vendor/benchmark.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/boost.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/cheap-ruler-cpp.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/earcut.hpp.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/eternal.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/geojson-vt-cpp.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/googletest.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/icu.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/nunicode.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/polylabel.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/protozero.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/shelf-pack-cpp.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/unique_resource.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/vector-tile.cmake)
include(${CMAKE_SOURCE_DIR}/vendor/wagyu.cmake)

if(NOT TARGET mapbox-base)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/mapbox-base ${CMAKE_BINARY_DIR}/.build-mapbox-gl-native-mapbox-base EXCLUDE_FROM_ALL)
endif()

if(MBGL_PLATFORM STREQUAL "linux" OR MBGL_PLATFORM STREQUAL "macos")
    include(${CMAKE_SOURCE_DIR}/vendor/glfw.cmake)
endif()
