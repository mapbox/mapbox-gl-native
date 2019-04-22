add_library(args INTERFACE)

target_include_directories(args SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/args
)
