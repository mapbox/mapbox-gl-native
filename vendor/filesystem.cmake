add_library(filesystem INTERFACE)

target_include_directories(filesystem SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/filesystem/include
)
