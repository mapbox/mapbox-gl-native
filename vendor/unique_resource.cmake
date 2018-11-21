add_library(unique_resource INTERFACE)

target_include_directories(unique_resource SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/unique_resource
)
