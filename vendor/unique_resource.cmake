if(TARGET mbgl-vendor-unique_resource)
    return()
endif()

add_library(mbgl-vendor-unique_resource INTERFACE)

target_include_directories(mbgl-vendor-unique_resource SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/unique_resource
)
