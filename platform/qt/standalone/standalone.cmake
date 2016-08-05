include(ExternalProject)

# Macro for cloning and downloading our header-only dependencies
macro(add_dep package tag git)
    externalproject_add(${package}
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/3rd_party/${package}
        STAMP_DIR ${CMAKE_SOURCE_DIR}/src/3rd_party/stamps
        URL https://github.com/${git}/archive/${tag}.zip
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )

    add_dependencies(mbgl-core ${package})

    externalproject_get_property(${package} SOURCE_DIR)

    target_include_directories(mbgl-core PRIVATE ${SOURCE_DIR})
    target_include_directories(mbgl-core PRIVATE ${SOURCE_DIR}/include)
    target_include_directories(qmapboxgl PRIVATE ${SOURCE_DIR})
    target_include_directories(qmapboxgl PRIVATE ${SOURCE_DIR}/include)

    set(${package}_SOURCE_DIR ${SOURCE_DIR})
endmacro()

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/include/mbgl/util/version.hpp
    DEPENDS ${MBGL_VERSION_DEPS}
    COMMAND node ${CMAKE_SOURCE_DIR}/scripts/build-version.js ${CMAKE_SOURCE_DIR}
    VERBATIM
)

macro(add_shader shader)
    set(shader_build_cmd node ${CMAKE_SOURCE_DIR}/scripts/build-shaders.js)
    set(shader_file_prefix ${mapbox-gl-shaders_SOURCE_DIR}/shaders)
    set(shader_source_prefix ${mapbox-gl-shaders_SOURCE_DIR}/include/mbgl/shader)

    add_custom_command(
       OUTPUT ${shader_source_prefix}/${shader}.hpp
       COMMAND ${shader_build_cmd} ${shader} ${shader_file_prefix} ${shader_source_prefix}
       DEPENDS ${shader_file_prefix}/${shader}.vertex.glsl
       DEPENDS ${shader_file_prefix}/${shader}.fragment.glsl
       DEPENDS ${shader_file_prefix}/_prelude.vertex.glsl
       DEPENDS ${shader_file_prefix}/_prelude.fragment.glsl
       DEPENDS mapbox-gl-shaders
       VERBATIM
    )

    set(MBGL_SHADER_FILES ${MBGL_SHADER_FILES} ${shader_source_prefix}/${shader}.hpp)
endmacro()
