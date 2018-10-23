add_library(geojson.hpp INTERFACE)

target_include_directories(geojson.hpp SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/geojson.hpp/include
)
