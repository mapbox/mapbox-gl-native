if(TARGET mbgl-vendor-geojson-vt-cpp)
    return()
endif()

add_library(mbgl-vendor-geojson-vt-cpp INTERFACE)

target_include_directories(mbgl-vendor-geojson-vt-cpp SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/geojson-vt-cpp/include
)
