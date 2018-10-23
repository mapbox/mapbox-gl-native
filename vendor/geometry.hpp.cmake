add_library(geometry.hpp INTERFACE)

target_include_directories(geometry.hpp SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/geometry.hpp/include
)
