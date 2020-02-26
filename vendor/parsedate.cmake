if(TARGET mbgl-vendor-parsedate)
    return()
endif()

add_library(
    mbgl-vendor-parsedate STATIC
    ${CMAKE_CURRENT_LIST_DIR}/parsedate/parsedate.cpp
)

target_link_libraries(
    mbgl-vendor-parsedate
    PRIVATE mbgl-compiler-options
)

target_include_directories(
    mbgl-vendor-parsedate SYSTEM
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/parsedate
)

set_target_properties(
    mbgl-vendor-parsedate
    PROPERTIES
        INTERFACE_MAPBOX_NAME "parsedate"
        INTERFACE_MAPBOX_URL "https://curl.haxx.se"
        INTERFACE_MAPBOX_AUTHOR "Daniel Stenberg and others"
        INTERFACE_MAPBOX_LICENSE ${CMAKE_CURRENT_LIST_DIR}/parsedate/COPYING
)
