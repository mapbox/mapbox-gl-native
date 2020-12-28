if(TARGET mbgl-vendor-cpp-httplib)
    return()
endif()

add_library(
    mbgl-vendor-cpp-httplib INTERFACE
)

target_include_directories(
    mbgl-vendor-cpp-httplib SYSTEM
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/cpp-httplib
)
