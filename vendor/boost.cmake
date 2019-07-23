if(TARGET mbgl-vendor-boost)
    return()
endif()

add_library(mbgl-vendor-boost INTERFACE)

target_include_directories(mbgl-vendor-boost SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/boost/include
)
