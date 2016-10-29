function(add_shader VAR name)
    set(shader_build_cmd ${NodeJS_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/build-shaders.js)
    set(shader_file_prefix ${CMAKE_SOURCE_DIR}/node_modules/mapbox-gl-shaders/src)
    set(shader_source_prefix ${MBGL_GENERATED}/include/mbgl/shader)

    add_custom_command(
       OUTPUT ${shader_source_prefix}/${name}.hpp
       COMMAND ${shader_build_cmd} ${shader_file_prefix}/${name}.vertex.glsl ${shader_file_prefix}/${name}.fragment.glsl ${shader_source_prefix}/${name}.hpp
       DEPENDS ${shader_file_prefix}/${name}.vertex.glsl
       DEPENDS ${shader_file_prefix}/${name}.fragment.glsl
       VERBATIM
    )
    set(${VAR} ${${VAR}} ${shader_source_prefix}/${name}.hpp PARENT_SCOPE)
endfunction()

add_shader(MBGL_SHADER_FILES circle)
add_shader(MBGL_SHADER_FILES collision_box)
add_shader(MBGL_SHADER_FILES debug)
add_shader(MBGL_SHADER_FILES fill)
add_shader(MBGL_SHADER_FILES fill_outline)
add_shader(MBGL_SHADER_FILES fill_outline_pattern)
add_shader(MBGL_SHADER_FILES fill_pattern)
add_shader(MBGL_SHADER_FILES line)
add_shader(MBGL_SHADER_FILES line_pattern)
add_shader(MBGL_SHADER_FILES line_sdf)
add_shader(MBGL_SHADER_FILES raster)
add_shader(MBGL_SHADER_FILES symbol_icon)
add_shader(MBGL_SHADER_FILES symbol_sdf)
