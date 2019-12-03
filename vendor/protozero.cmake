if(TARGET mbgl-vendor-protozero)
    return()
endif()

add_library(mbgl-vendor-protozero INTERFACE)

target_include_directories(mbgl-vendor-protozero SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/protozero/include
)
