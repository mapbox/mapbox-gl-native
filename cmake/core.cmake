# Modify cmake/core-files.txt to change the source files for this target.
load_sources_list(MBGL_CORE_FILES cmake/core-files.txt)
add_library(mbgl-core STATIC ${MBGL_CORE_FILES})

target_include_directories(mbgl-core
    PUBLIC include
    PRIVATE src
)

target_link_libraries(mbgl-core PRIVATE
    earcut.hpp
    expected
    geojson-vt-cpp
    kdbush.hpp
    shelf-pack-cpp
    supercluster.hpp
    unique_resource
    wagyu
)

# FIXME: We should not leak these many
# libraries in our public interface.
target_link_libraries(mbgl-core PUBLIC
    boost
    geojson.hpp
    geometry.hpp
    icu
    optional
    polylabel
    protozero
    rapidjson
    variant
    vector-tile
)

mbgl_platform_core()

create_source_groups(mbgl-core)

set_target_properties(mbgl-core PROPERTIES FOLDER "Core")

xcode_create_scheme(TARGET mbgl-core)

initialize_xcode_cxx_build_settings(mbgl-core)
