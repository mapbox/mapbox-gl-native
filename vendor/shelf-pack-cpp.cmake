if(TARGET mbgl-vendor-shelf-pack-cpp)
    return()
endif()

add_library(mbgl-vendor-shelf-pack-cpp INTERFACE)

target_include_directories(mbgl-vendor-shelf-pack-cpp SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/shelf-pack-cpp/include
)
