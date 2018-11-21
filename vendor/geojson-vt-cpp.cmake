add_library(geojson-vt-cpp INTERFACE)

target_include_directories(geojson-vt-cpp SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/geojson-vt-cpp/include
)
