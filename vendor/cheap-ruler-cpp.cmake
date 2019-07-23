if(TARGET mbgl-vendor-cheap-ruler-cpp)
    return()
endif()

add_library(mbgl-vendor-cheap-ruler-cpp INTERFACE)

target_include_directories(mbgl-vendor-cheap-ruler-cpp SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/cheap-ruler-cpp/include
)

