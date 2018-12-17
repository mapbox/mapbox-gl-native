add_library(shelf-pack-cpp INTERFACE)

target_include_directories(shelf-pack-cpp SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/shelf-pack-cpp/include
)
