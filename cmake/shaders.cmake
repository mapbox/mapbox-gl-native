function(add_shader VAR name)
    set(shader_build_cmd ${CMAKE_SOURCE_DIR}/scripts/build-shaders.py)
    set(shader_file_prefix ${CMAKE_SOURCE_DIR}/node_modules/mapbox-gl-shaders/src)
    set(shader_source_prefix ${MBGL_GENERATED}/include/mbgl/shader)

    add_custom_command(
       OUTPUT ${shader_source_prefix}/${name}.vertex.hpp ${shader_source_prefix}/${name}.fragment.hpp
       COMMAND ${shader_build_cmd} ${shader_file_prefix}/${name}.vertex.glsl ${shader_source_prefix}/${name}.vertex.hpp
       COMMAND ${shader_build_cmd} ${shader_file_prefix}/${name}.fragment.glsl ${shader_source_prefix}/${name}.fragment.hpp
       DEPENDS ${shader_file_prefix}/${name}.vertex.glsl
       DEPENDS ${shader_file_prefix}/${name}.fragment.glsl
       VERBATIM
    )
    set(${VAR} ${${VAR}} ${shader_source_prefix}/${name}.vertex.hpp ${shader_source_prefix}/${name}.fragment.hpp PARENT_SCOPE)
endfunction()

add_shader(MBGL_SHADER_FILES circle)
add_shader(MBGL_SHADER_FILES collisionbox)
add_shader(MBGL_SHADER_FILES debug)
add_shader(MBGL_SHADER_FILES fill)
add_shader(MBGL_SHADER_FILES icon)
add_shader(MBGL_SHADER_FILES line)
add_shader(MBGL_SHADER_FILES linepattern)
add_shader(MBGL_SHADER_FILES linesdfpattern)
add_shader(MBGL_SHADER_FILES outline)
add_shader(MBGL_SHADER_FILES outlinepattern)
add_shader(MBGL_SHADER_FILES pattern)
add_shader(MBGL_SHADER_FILES raster)
add_shader(MBGL_SHADER_FILES sdf)
