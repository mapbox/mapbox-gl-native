if(TARGET mbgl-vendor-polylabel)
    return()
endif()

add_library(mbgl-vendor-polylabel INTERFACE)

target_include_directories(mbgl-vendor-polylabel SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/polylabel/include
)
