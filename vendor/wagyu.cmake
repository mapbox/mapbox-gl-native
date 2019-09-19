if(TARGET mbgl-vendor-wagyu)
    return()
endif()

add_library(mbgl-vendor-wagyu INTERFACE)

target_include_directories(mbgl-vendor-wagyu SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/wagyu/include
)
