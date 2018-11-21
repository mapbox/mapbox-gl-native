add_library(earcut.hpp INTERFACE)

target_include_directories(earcut.hpp SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/earcut.hpp/include
)
