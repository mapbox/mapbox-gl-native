if(__MBGL_GFX__)
    message("Graphics backend already defined, not building OpenGL GFX")
    return()
endif()

set(__MBGL_GFX__ TRUE)

target_sources(mbgl-core PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/mbgl/gl/renderable_resource.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/mbgl/gl/renderer_backend.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/mbgl/platform/gl_functions.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/attribute.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/attribute.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/command_encoder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/command_encoder.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/context.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/context.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/debugging_extension.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/debugging_extension.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/defines.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/draw_scope_resource.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/enum.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/enum.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/extension.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/framebuffer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/index_buffer_resource.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/object.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/object.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/offscreen_texture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/offscreen_texture.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/program.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/render_pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/render_pass.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/renderbuffer_resource.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/renderer_backend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/state.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/texture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/texture.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/texture_resource.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/types.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/uniform.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/uniform.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/upload_pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/upload_pass.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/value.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/value.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/vertex_array.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/vertex_array.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/vertex_array_extension.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/gl/vertex_buffer_resource.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/platform/gl_functions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/background.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/background_pattern.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/circle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/clipping_mask.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/collision_box.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/collision_circle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/debug.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/fill.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/fill_extrusion.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/fill_extrusion_pattern.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/fill_outline.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/fill_outline_pattern.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/fill_pattern.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/heatmap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/heatmap_texture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/hillshade.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/hillshade_prepare.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/line.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/line_gradient.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/line_pattern.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/line_sdf.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/preludes.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/raster.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/shader_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/shader_source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/shaders.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/shaders.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/symbol_icon.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/symbol_sdf_icon.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/programs/gl/symbol_sdf_text.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/renderer/layers/render_custom_layer.cpp
)

target_include_directories(mbgl-core PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_include_directories(mbgl-core PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
