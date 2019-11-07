# scripts/generate-file-lists.js to change the source files for this target.
load_sources_list(MBGL_CORE_FILES src/core-files.json)
add_library(mbgl-core STATIC ${MBGL_CORE_FILES})

target_include_directories(mbgl-core
    PUBLIC include
    PRIVATE src
)

target_link_libraries(mbgl-core PRIVATE
    mbgl-vendor-earcut.hpp
    mbgl-vendor-eternal
    mbgl-vendor-geojson-vt-cpp
    mbgl-vendor-shelf-pack-cpp
    mbgl-vendor-unique_resource
    mbgl-vendor-wagyu
)

# linux uses ICU from mason, other platforms use vendored ICU
if(NOT MBGL_PLATFORM STREQUAL "linux")
    set(ICU_LIBRARY "mbgl-vendor-icu")
endif()

# FIXME: We should not leak these many
# libraries in our public interface.
target_link_libraries(mbgl-core PUBLIC
    mbgl-vendor-boost
    mapbox-base-extras
    mapbox-base
    ${ICU_LIBRARY}
    mbgl-vendor-polylabel
    mbgl-vendor-protozero
    Mapbox::Base::Extras::rapidjson
    Mapbox::Base::Extras::expected-lite
    mbgl-vendor-vector-tile
)

mbgl_platform_core()

create_source_groups(mbgl-core)

set_target_properties(mbgl-core PROPERTIES FOLDER "Core")

xcode_create_scheme(TARGET mbgl-core)

initialize_xcode_cxx_build_settings(mbgl-core)
