message("-- Configuring GL-Native with Metal renderer backend")

if(NOT EXISTS ${PROJECT_SOURCE_DIR}/modules/metal)
    execute_process(
        COMMAND git clone git@github.com:mapbox/mapbox-maps-metal.git ${PROJECT_SOURCE_DIR}/modules/metal
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endif()

target_compile_definitions(
    mbgl-core
    PRIVATE MBGL_RENDER_BACKEND_METAL=1
)

target_sources(mbgl-core PRIVATE
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/buffer_resource.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/command_encoder.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/context.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/conversion.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/draw_scope_resource.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/offscreen_texture.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/program.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/render_pass.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/renderable_resource.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/renderbuffer_resource.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/renderer_backend.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/texture_resource.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/upload_pass.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/background.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/background_pattern.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/circle.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/clipping_mask.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/collision_box.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/collision_circle.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/debug.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/fill.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/fill_extrusion.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/fill_extrusion_pattern.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/fill_outline.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/fill_outline_pattern.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/fill_pattern.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/heatmap.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/heatmap_texture.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/hillshade.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/hillshade_prepare.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/line.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/line_gradient.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/line_pattern.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/line_sdf.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/raster.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/symbol_icon.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/symbol_sdf_icon.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/symbol_sdf_text.mm
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/programs/metal/symbol_text_and_icon.mm
)

target_sources(mbgl-core PUBLIC
    ${PROJECT_SOURCE_DIR}/modules/metal/include/mbgl/gfx/backend.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/include/mbgl/metal/configuration.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/include/mbgl/metal/renderable_resource.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/include/mbgl/metal/renderer_backend.hpp
)

target_sources(mbgl-core PRIVATE
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/attribute.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/buffer_resource.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/command_encoder.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/context.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/conversion.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/draw_scope_resource.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/index_buffer_resource.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/offscreen_texture.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/program.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/render_pass.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/renderbuffer_resource.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/shader.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/texture_resource.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/upload_pass.hpp
    ${PROJECT_SOURCE_DIR}/modules/metal/src/mbgl/metal/vertex_buffer_resource.hpp
)

target_sources(mbgl-core PRIVATE
    ${PROJECT_SOURCE_DIR}/modules/metal/platform/darwin/src/headless_backend.mm
)

target_include_directories(mbgl-core BEFORE
    PRIVATE ${PROJECT_SOURCE_DIR}/modules/metal/src
    PUBLIC  ${PROJECT_SOURCE_DIR}/modules/metal/include
)

target_include_directories(mbgl-core PRIVATE
    ${PROJECT_SOURCE_DIR}/platform/default/include
)

target_link_libraries(mbgl-core
    PUBLIC "-framework MetalKit"
    PUBLIC "-framework Metal"
)
