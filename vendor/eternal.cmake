if(TARGET mbgl-vendor-eternal)
    return()
endif()

add_library(mbgl-vendor-eternal INTERFACE)

target_include_directories(mbgl-vendor-eternal SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/eternal/include
)
