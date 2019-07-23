if(TARGET mbgl-vendor-expected)
    return()
endif()

add_library(mbgl-vendor-expected INTERFACE)

target_include_directories(mbgl-vendor-expected SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/expected/include
)
