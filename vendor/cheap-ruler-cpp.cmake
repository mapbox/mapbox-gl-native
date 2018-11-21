add_library(cheap-ruler-cpp INTERFACE)

target_include_directories(cheap-ruler-cpp SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/cheap-ruler-cpp/include
)

