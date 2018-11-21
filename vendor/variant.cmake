add_library(variant INTERFACE)

target_include_directories(variant SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/variant/include
)
