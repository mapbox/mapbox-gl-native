if(TARGET mbgl-vendor-vector-tile)
    return()
endif()

add_library(mbgl-vendor-vector-tile INTERFACE)

target_include_directories(mbgl-vendor-vector-tile SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/vector-tile/include
)
