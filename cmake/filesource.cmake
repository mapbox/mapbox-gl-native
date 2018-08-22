add_vendor_target(expected INTERFACE)

# Modify cmake/filesource-files.txt to change the source files for this target.
load_sources_list(MBGL_FILESOURCE_FILES cmake/filesource-files.txt)
add_library(mbgl-filesource STATIC ${MBGL_FILESOURCE_FILES})

target_add_mason_package(mbgl-filesource PUBLIC geometry)
target_add_mason_package(mbgl-filesource PUBLIC variant)
target_add_mason_package(mbgl-filesource PRIVATE rapidjson)
target_add_mason_package(mbgl-filesource PRIVATE boost)
target_add_mason_package(mbgl-filesource PRIVATE geojson)

target_include_directories(mbgl-filesource
    PRIVATE include
    PRIVATE src
    PRIVATE platform/default
)

target_link_libraries(mbgl-filesource
    PUBLIC mbgl-core
    PUBLIC expected
)

mbgl_filesource()

create_source_groups(mbgl-filesource)

set_target_properties(mbgl-filesource PROPERTIES FOLDER "Core")

xcode_create_scheme(TARGET mbgl-filesource)

initialize_xcode_cxx_build_settings(mbgl-filesource)
