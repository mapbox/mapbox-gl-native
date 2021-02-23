if(TARGET mbgl-vendor-vector-tile)
    return()
endif()

add_library(
    mbgl-vendor-vector-tile INTERFACE
)

target_include_directories(
    mbgl-vendor-vector-tile SYSTEM
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/vector-tile/include
)

set_target_properties(
    mbgl-vendor-vector-tile
    PROPERTIES
        INTERFACE_MAPBOX_NAME "vector-tile"
        INTERFACE_MAPBOX_URL "https://github.com/mapbox/vector-tile"
        INTERFACE_MAPBOX_AUTHOR "Mapbox"
        INTERFACE_MAPBOX_LICENSE ${CMAKE_CURRENT_LIST_DIR}/vector-tile/LICENSE
)
