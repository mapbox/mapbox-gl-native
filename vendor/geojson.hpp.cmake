add_library(geojson.hpp INTERFACE)

target_include_directories(geojson.hpp SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/geojson.hpp/include
)
