add_library(geojson-vt-cpp INTERFACE)

target_include_directories(geojson-vt-cpp SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/geojson-vt-cpp/include
)
