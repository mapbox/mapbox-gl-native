if(TARGET mbgl-vendor-boost)
    return()
endif()

add_library(
    mbgl-vendor-boost INTERFACE
)

target_include_directories(
    mbgl-vendor-boost SYSTEM
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/boost/include
)

set_target_properties(
    mbgl-vendor-boost
    PROPERTIES
        INTERFACE_MAPBOX_NAME "Boost C++ Libraries"
        INTERFACE_MAPBOX_URL "https://www.boost.org"
        INTERFACE_MAPBOX_AUTHOR "Boost authors"
        INTERFACE_MAPBOX_LICENSE ${CMAKE_CURRENT_LIST_DIR}/boost/LICENSE_1_0.txt
)
