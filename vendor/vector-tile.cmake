add_library(vector-tile INTERFACE)

target_include_directories(vector-tile SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/vector-tile/include
)
