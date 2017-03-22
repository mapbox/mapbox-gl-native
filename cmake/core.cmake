add_library(mbgl-core STATIC
    ${MBGL_CORE_FILES}
)

target_compile_options(mbgl-core
    PRIVATE -fPIC
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-core
    PUBLIC include
    PRIVATE src
)

target_add_mason_package(mbgl-core PUBLIC geometry)
target_add_mason_package(mbgl-core PUBLIC variant)
target_add_mason_package(mbgl-core PUBLIC unique_resource)
target_add_mason_package(mbgl-core PRIVATE rapidjson)
target_add_mason_package(mbgl-core PRIVATE boost)
target_add_mason_package(mbgl-core PRIVATE geojson)
target_add_mason_package(mbgl-core PRIVATE geojsonvt)
target_add_mason_package(mbgl-core PRIVATE supercluster)
target_add_mason_package(mbgl-core PRIVATE kdbush)
target_add_mason_package(mbgl-core PRIVATE earcut)
target_add_mason_package(mbgl-core PRIVATE protozero)
target_add_mason_package(mbgl-core PRIVATE polylabel)
target_add_mason_package(mbgl-core PRIVATE wagyu)

mbgl_platform_core()

create_source_groups(mbgl-core)
target_append_xcconfig(mbgl-core)
