# scripts/generate-file-lists.js to change the source files for this target.
load_sources_list(MBGL_CORE_FILES src/core-files.json)
add_library(mbgl-core STATIC ${MBGL_CORE_FILES})

target_include_directories(mbgl-core
    PUBLIC include
    PRIVATE src
)

target_link_libraries(mbgl-core PRIVATE
    earcut.hpp
    eternal
    expected
    geojson-vt-cpp
    kdbush.hpp
    shelf-pack-cpp
    supercluster.hpp
    unique_resource
    wagyu
)

# linux uses ICU from mason, other platforms use vendored ICU
if(NOT MBGL_PLATFORM STREQUAL "linux")
    set(ICU_LIBRARY "icu")
endif()

# FIXME: We should not leak these many
# libraries in our public interface.
target_link_libraries(mbgl-core PUBLIC
    boost
    mapbox-base
    geojson.hpp
    ${ICU_LIBRARY}
    polylabel
    protozero
    rapidjson
    vector-tile
)

mbgl_platform_core()

create_source_groups(mbgl-core)

set_target_properties(mbgl-core PROPERTIES FOLDER "Core")

xcode_create_scheme(TARGET mbgl-core)

initialize_xcode_cxx_build_settings(mbgl-core)
