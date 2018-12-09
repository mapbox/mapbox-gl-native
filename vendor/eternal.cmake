add_library(eternal INTERFACE)

target_include_directories(eternal SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/eternal/include
)
