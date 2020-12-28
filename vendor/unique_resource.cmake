if(TARGET mbgl-vendor-unique_resource)
    return()
endif()

add_library(
    mbgl-vendor-unique_resource INTERFACE
)

target_include_directories(
    mbgl-vendor-unique_resource SYSTEM
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/unique_resource
)

set_target_properties(
    mbgl-vendor-unique_resource
    PROPERTIES
        INTERFACE_MAPBOX_NAME "unique_resource"
        INTERFACE_MAPBOX_URL "https://github.com/okdshin/unique_resource"
        INTERFACE_MAPBOX_AUTHOR "Shintarou Okada"
        INTERFACE_MAPBOX_LICENSE ${CMAKE_CURRENT_LIST_DIR}/unique_resource/LICENSE.txt
)
