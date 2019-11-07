# Modify platform/default/filesource-files.json to change the source files for this target.
load_sources_list(MBGL_FILESOURCE_FILES platform/default/filesource-files.json)
add_library(mbgl-filesource STATIC ${MBGL_FILESOURCE_FILES})

target_include_directories(mbgl-filesource
    PRIVATE include
    PRIVATE src
    PRIVATE platform/default/include
)

target_link_libraries(mbgl-filesource
    PUBLIC mbgl-core
    PUBLIC Mapbox::Base::Extras::expected-lite
)

mbgl_filesource()

create_source_groups(mbgl-filesource)

set_target_properties(mbgl-filesource PROPERTIES FOLDER "Core")

xcode_create_scheme(TARGET mbgl-filesource)

initialize_xcode_cxx_build_settings(mbgl-filesource)
