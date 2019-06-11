add_library(optional INTERFACE)

target_include_directories(optional SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/optional
)
