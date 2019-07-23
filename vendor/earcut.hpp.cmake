if(TARGET mbgl-vendor-earcut.hpp)
    return()
endif()

add_library(mbgl-vendor-earcut.hpp INTERFACE)

target_include_directories(mbgl-vendor-earcut.hpp SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/earcut.hpp/include
)
