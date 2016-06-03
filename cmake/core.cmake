set(MBGL_VERSION_DEPS package.json)
if(EXISTS ${CMAKE_SOURCE_DIR}/.git/HEAD)
    set(MBGL_VERSION_DEPS ${MBGL_VERSION_DEPS} .git/HEAD)
endif()

add_custom_command(
    OUTPUT ${MBGL_GENERATED}/include/mbgl/util/version.hpp
    DEPENDS ${MBGL_VERSION_DEPS}
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/build-version.py ${MBGL_GENERATED}
    VERBATIM
)

add_custom_target(mbgl-headers DEPENDS
    ${MBGL_GENERATED}/include/mbgl/util/version.hpp
    ${MBGL_SHADER_FILES}
)

add_library(mbgl-core STATIC
    ${MBGL_CORE_FILES}
)

add_dependencies(mbgl-core
    mbgl-headers
)

target_compile_options(mbgl-core
    PRIVATE -fPIC
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-core
    PUBLIC include
    PUBLIC src # TODO: make private
    PRIVATE ${MBGL_GENERATED}/include
)

target_add_mason_package(mbgl-core PUBLIC geometry)
target_add_mason_package(mbgl-core PUBLIC variant)
target_add_mason_package(mbgl-core PRIVATE unique_resource)
target_add_mason_package(mbgl-core PRIVATE rapidjson)
target_add_mason_package(mbgl-core PRIVATE boost)
target_add_mason_package(mbgl-core PRIVATE geojson)
target_add_mason_package(mbgl-core PRIVATE geojsonvt)
target_add_mason_package(mbgl-core PRIVATE supercluster)
target_add_mason_package(mbgl-core PRIVATE kdbush)
target_add_mason_package(mbgl-core PRIVATE earcut)
target_add_mason_package(mbgl-core PRIVATE protozero)

mbgl_platform_core()

create_source_groups(mbgl-core)
target_append_xcconfig(mbgl-core)
