if(TARGET mbgl-vendor-eternal)
    return()
endif()

add_library(
    mbgl-vendor-eternal INTERFACE
)

target_include_directories(
    mbgl-vendor-eternal SYSTEM
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/eternal/include
)

set_target_properties(
    mbgl-vendor-eternal
    PROPERTIES
        INTERFACE_MAPBOX_NAME "eternal"
        INTERFACE_MAPBOX_URL "https://github.com/mapbox/eternal"
        INTERFACE_MAPBOX_AUTHOR "Mapbox"
        INTERFACE_MAPBOX_LICENSE ${CMAKE_CURRENT_LIST_DIR}/eternal/LICENSE.md
)
