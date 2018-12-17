add_library(expected INTERFACE)

target_include_directories(expected SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/expected/include
)
