# Do not edit. Regenerate this with ./scripts/generate-core-files.sh

set(MBGL_CORE_FILES
    # actor
    include/mbgl/actor/actor.hpp
    include/mbgl/actor/actor_ref.hpp
    include/mbgl/actor/mailbox.hpp
    include/mbgl/actor/message.hpp
    include/mbgl/actor/scheduler.hpp
    src/mbgl/actor/mailbox.cpp
    src/mbgl/actor/scheduler.cpp

    # algorithm
    src/mbgl/algorithm/covered_by_children.hpp
    src/mbgl/algorithm/generate_clip_ids.cpp
    src/mbgl/algorithm/generate_clip_ids.hpp
    src/mbgl/algorithm/generate_clip_ids_impl.hpp
    src/mbgl/algorithm/update_renderables.hpp
    src/mbgl/algorithm/update_tile_masks.hpp

    # annotation
    include/mbgl/annotation/annotation.hpp
    src/mbgl/annotation/annotation_manager.cpp
    src/mbgl/annotation/annotation_manager.hpp
    src/mbgl/annotation/annotation_source.cpp
    src/mbgl/annotation/annotation_source.hpp
    src/mbgl/annotation/annotation_tile.cpp
    src/mbgl/annotation/annotation_tile.hpp
    src/mbgl/annotation/fill_annotation_impl.cpp
    src/mbgl/annotation/fill_annotation_impl.hpp
    src/mbgl/annotation/line_annotation_impl.cpp
    src/mbgl/annotation/line_annotation_impl.hpp
    src/mbgl/annotation/render_annotation_source.cpp
    src/mbgl/annotation/render_annotation_source.hpp
    src/mbgl/annotation/shape_annotation_impl.cpp
    src/mbgl/annotation/shape_annotation_impl.hpp
    src/mbgl/annotation/symbol_annotation_impl.cpp
    src/mbgl/annotation/symbol_annotation_impl.hpp

    # csscolorparser
    src/csscolorparser/csscolorparser.cpp
    src/csscolorparser/csscolorparser.hpp

    # geometry
    src/mbgl/geometry/anchor.hpp
    src/mbgl/geometry/debug_font_data.hpp
    src/mbgl/geometry/feature_index.cpp
    src/mbgl/geometry/feature_index.hpp
    src/mbgl/geometry/line_atlas.cpp
    src/mbgl/geometry/line_atlas.hpp

    # gl
    src/mbgl/gl/attribute.cpp
    src/mbgl/gl/attribute.hpp
    src/mbgl/gl/color_mode.cpp
    src/mbgl/gl/color_mode.hpp
    src/mbgl/gl/context.cpp
    src/mbgl/gl/context.hpp
    src/mbgl/gl/debugging.cpp
    src/mbgl/gl/debugging.hpp
    src/mbgl/gl/debugging_extension.cpp
    src/mbgl/gl/debugging_extension.hpp
    src/mbgl/gl/depth_mode.cpp
    src/mbgl/gl/depth_mode.hpp
    src/mbgl/gl/draw_mode.hpp
    src/mbgl/gl/extension.hpp
    src/mbgl/gl/features.hpp
    src/mbgl/gl/framebuffer.hpp
    src/mbgl/gl/gl.cpp
    src/mbgl/gl/gl.hpp
    src/mbgl/gl/index_buffer.hpp
    src/mbgl/gl/object.cpp
    src/mbgl/gl/object.hpp
    src/mbgl/gl/primitives.hpp
    src/mbgl/gl/program.hpp
    src/mbgl/gl/program_binary_extension.hpp
    src/mbgl/gl/renderbuffer.hpp
    src/mbgl/gl/state.hpp
    src/mbgl/gl/stencil_mode.cpp
    src/mbgl/gl/stencil_mode.hpp
    src/mbgl/gl/texture.hpp
    src/mbgl/gl/types.hpp
    src/mbgl/gl/uniform.cpp
    src/mbgl/gl/uniform.hpp
    src/mbgl/gl/value.cpp
    src/mbgl/gl/value.hpp
    src/mbgl/gl/vertex_array.cpp
    src/mbgl/gl/vertex_array.hpp
    src/mbgl/gl/vertex_array_extension.hpp
    src/mbgl/gl/vertex_buffer.hpp

    # layout
    src/mbgl/layout/clip_lines.cpp
    src/mbgl/layout/clip_lines.hpp
    src/mbgl/layout/merge_lines.cpp
    src/mbgl/layout/merge_lines.hpp
    src/mbgl/layout/symbol_feature.hpp
    src/mbgl/layout/symbol_instance.cpp
    src/mbgl/layout/symbol_instance.hpp
    src/mbgl/layout/symbol_layout.cpp
    src/mbgl/layout/symbol_layout.hpp
    src/mbgl/layout/symbol_projection.cpp
    src/mbgl/layout/symbol_projection.hpp

    # map
    include/mbgl/map/camera.hpp
    include/mbgl/map/change.hpp
    include/mbgl/map/map.hpp
    include/mbgl/map/map_observer.hpp
    include/mbgl/map/mode.hpp
    src/mbgl/map/map.cpp
    src/mbgl/map/transform.cpp
    src/mbgl/map/transform.hpp
    src/mbgl/map/transform_state.cpp
    src/mbgl/map/transform_state.hpp
    src/mbgl/map/zoom_history.hpp

    # math
    include/mbgl/math/clamp.hpp
    include/mbgl/math/log2.hpp
    include/mbgl/math/minmax.hpp
    include/mbgl/math/wrap.hpp
    src/mbgl/math/log2.cpp

    # parsedate
    src/parsedate/parsedate.c
    src/parsedate/parsedate.h

    # programs
    src/mbgl/programs/attributes.hpp
    src/mbgl/programs/binary_program.cpp
    src/mbgl/programs/binary_program.hpp
    src/mbgl/programs/circle_program.cpp
    src/mbgl/programs/circle_program.hpp
    src/mbgl/programs/collision_box_program.cpp
    src/mbgl/programs/collision_box_program.hpp
    src/mbgl/programs/debug_program.hpp
    src/mbgl/programs/extrusion_texture_program.cpp
    src/mbgl/programs/extrusion_texture_program.hpp
    src/mbgl/programs/fill_extrusion_program.cpp
    src/mbgl/programs/fill_extrusion_program.hpp
    src/mbgl/programs/fill_program.cpp
    src/mbgl/programs/fill_program.hpp
    src/mbgl/programs/line_program.cpp
    src/mbgl/programs/line_program.hpp
    src/mbgl/programs/program.hpp
    src/mbgl/programs/program_parameters.cpp
    src/mbgl/programs/program_parameters.hpp
    src/mbgl/programs/programs.hpp
    src/mbgl/programs/raster_program.cpp
    src/mbgl/programs/raster_program.hpp
    src/mbgl/programs/segment.hpp
    src/mbgl/programs/symbol_program.cpp
    src/mbgl/programs/symbol_program.hpp
    src/mbgl/programs/uniforms.hpp

    # renderer
    include/mbgl/renderer/backend_scope.hpp
    include/mbgl/renderer/query.hpp
    include/mbgl/renderer/renderer.hpp
    include/mbgl/renderer/renderer_backend.hpp
    include/mbgl/renderer/renderer_frontend.hpp
    src/mbgl/renderer/backend_scope.cpp
    src/mbgl/renderer/bucket.hpp
    src/mbgl/renderer/bucket_parameters.cpp
    src/mbgl/renderer/bucket_parameters.hpp
    src/mbgl/renderer/cross_faded_property_evaluator.cpp
    src/mbgl/renderer/cross_faded_property_evaluator.hpp
    src/mbgl/renderer/data_driven_property_evaluator.hpp
    src/mbgl/renderer/frame_history.cpp
    src/mbgl/renderer/frame_history.hpp
    src/mbgl/renderer/group_by_layout.cpp
    src/mbgl/renderer/group_by_layout.hpp
    src/mbgl/renderer/image_atlas.cpp
    src/mbgl/renderer/image_atlas.hpp
    src/mbgl/renderer/image_manager.cpp
    src/mbgl/renderer/image_manager.hpp
    src/mbgl/renderer/paint_parameters.cpp
    src/mbgl/renderer/paint_parameters.hpp
    src/mbgl/renderer/paint_property_binder.hpp
    src/mbgl/renderer/paint_property_statistics.hpp
    src/mbgl/renderer/possibly_evaluated_property_value.hpp
    src/mbgl/renderer/property_evaluation_parameters.hpp
    src/mbgl/renderer/property_evaluator.hpp
    src/mbgl/renderer/render_layer.cpp
    src/mbgl/renderer/render_layer.hpp
    src/mbgl/renderer/render_light.cpp
    src/mbgl/renderer/render_light.hpp
    src/mbgl/renderer/render_pass.hpp
    src/mbgl/renderer/render_source.cpp
    src/mbgl/renderer/render_source.hpp
    src/mbgl/renderer/render_source_observer.hpp
    src/mbgl/renderer/render_static_data.cpp
    src/mbgl/renderer/render_static_data.hpp
    src/mbgl/renderer/render_tile.cpp
    src/mbgl/renderer/render_tile.hpp
    src/mbgl/renderer/renderer.cpp
    src/mbgl/renderer/renderer_backend.cpp
    src/mbgl/renderer/renderer_impl.cpp
    src/mbgl/renderer/renderer_impl.hpp
    src/mbgl/renderer/renderer_observer.hpp
    src/mbgl/renderer/style_diff.cpp
    src/mbgl/renderer/style_diff.hpp
    src/mbgl/renderer/tile_mask.hpp
    src/mbgl/renderer/tile_parameters.hpp
    src/mbgl/renderer/tile_pyramid.cpp
    src/mbgl/renderer/tile_pyramid.hpp
    src/mbgl/renderer/transition_parameters.hpp
    src/mbgl/renderer/update_parameters.hpp

    # renderer/buckets
    src/mbgl/renderer/buckets/circle_bucket.cpp
    src/mbgl/renderer/buckets/circle_bucket.hpp
    src/mbgl/renderer/buckets/debug_bucket.cpp
    src/mbgl/renderer/buckets/debug_bucket.hpp
    src/mbgl/renderer/buckets/fill_bucket.cpp
    src/mbgl/renderer/buckets/fill_bucket.hpp
    src/mbgl/renderer/buckets/fill_extrusion_bucket.cpp
    src/mbgl/renderer/buckets/fill_extrusion_bucket.hpp
    src/mbgl/renderer/buckets/line_bucket.cpp
    src/mbgl/renderer/buckets/line_bucket.hpp
    src/mbgl/renderer/buckets/raster_bucket.cpp
    src/mbgl/renderer/buckets/raster_bucket.hpp
    src/mbgl/renderer/buckets/symbol_bucket.cpp
    src/mbgl/renderer/buckets/symbol_bucket.hpp

    # renderer/layers
    src/mbgl/renderer/layers/render_background_layer.cpp
    src/mbgl/renderer/layers/render_background_layer.hpp
    src/mbgl/renderer/layers/render_circle_layer.cpp
    src/mbgl/renderer/layers/render_circle_layer.hpp
    src/mbgl/renderer/layers/render_custom_layer.cpp
    src/mbgl/renderer/layers/render_custom_layer.hpp
    src/mbgl/renderer/layers/render_fill_extrusion_layer.cpp
    src/mbgl/renderer/layers/render_fill_extrusion_layer.hpp
    src/mbgl/renderer/layers/render_fill_layer.cpp
    src/mbgl/renderer/layers/render_fill_layer.hpp
    src/mbgl/renderer/layers/render_line_layer.cpp
    src/mbgl/renderer/layers/render_line_layer.hpp
    src/mbgl/renderer/layers/render_raster_layer.cpp
    src/mbgl/renderer/layers/render_raster_layer.hpp
    src/mbgl/renderer/layers/render_symbol_layer.cpp
    src/mbgl/renderer/layers/render_symbol_layer.hpp

    # renderer/sources
    src/mbgl/renderer/sources/render_geojson_source.cpp
    src/mbgl/renderer/sources/render_geojson_source.hpp
    src/mbgl/renderer/sources/render_image_source.cpp
    src/mbgl/renderer/sources/render_image_source.hpp
    src/mbgl/renderer/sources/render_raster_source.cpp
    src/mbgl/renderer/sources/render_raster_source.hpp
    src/mbgl/renderer/sources/render_vector_source.cpp
    src/mbgl/renderer/sources/render_vector_source.hpp

    # shaders
    src/mbgl/shaders/circle.cpp
    src/mbgl/shaders/circle.hpp
    src/mbgl/shaders/collision_box.cpp
    src/mbgl/shaders/collision_box.hpp
    src/mbgl/shaders/debug.cpp
    src/mbgl/shaders/debug.hpp
    src/mbgl/shaders/extrusion_texture.cpp
    src/mbgl/shaders/extrusion_texture.hpp
    src/mbgl/shaders/fill.cpp
    src/mbgl/shaders/fill.hpp
    src/mbgl/shaders/fill_extrusion.cpp
    src/mbgl/shaders/fill_extrusion.hpp
    src/mbgl/shaders/fill_extrusion_pattern.cpp
    src/mbgl/shaders/fill_extrusion_pattern.hpp
    src/mbgl/shaders/fill_outline.cpp
    src/mbgl/shaders/fill_outline.hpp
    src/mbgl/shaders/fill_outline_pattern.cpp
    src/mbgl/shaders/fill_outline_pattern.hpp
    src/mbgl/shaders/fill_pattern.cpp
    src/mbgl/shaders/fill_pattern.hpp
    src/mbgl/shaders/line.cpp
    src/mbgl/shaders/line.hpp
    src/mbgl/shaders/line_pattern.cpp
    src/mbgl/shaders/line_pattern.hpp
    src/mbgl/shaders/line_sdf.cpp
    src/mbgl/shaders/line_sdf.hpp
    src/mbgl/shaders/preludes.cpp
    src/mbgl/shaders/preludes.hpp
    src/mbgl/shaders/raster.cpp
    src/mbgl/shaders/raster.hpp
    src/mbgl/shaders/shaders.cpp
    src/mbgl/shaders/shaders.hpp
    src/mbgl/shaders/symbol_icon.cpp
    src/mbgl/shaders/symbol_icon.hpp
    src/mbgl/shaders/symbol_sdf.cpp
    src/mbgl/shaders/symbol_sdf.hpp

    # sprite
    src/mbgl/sprite/sprite_loader.cpp
    src/mbgl/sprite/sprite_loader.hpp
    src/mbgl/sprite/sprite_loader_observer.hpp
    src/mbgl/sprite/sprite_loader_worker.cpp
    src/mbgl/sprite/sprite_loader_worker.hpp
    src/mbgl/sprite/sprite_parser.cpp
    src/mbgl/sprite/sprite_parser.hpp

    # storage
    include/mbgl/storage/default_file_source.hpp
    include/mbgl/storage/file_source.hpp
    include/mbgl/storage/network_status.hpp
    include/mbgl/storage/offline.hpp
    include/mbgl/storage/online_file_source.hpp
    include/mbgl/storage/resource.hpp
    include/mbgl/storage/resource_transform.hpp
    include/mbgl/storage/response.hpp
    src/mbgl/storage/asset_file_source.hpp
    src/mbgl/storage/http_file_source.hpp
    src/mbgl/storage/local_file_source.hpp
    src/mbgl/storage/network_status.cpp
    src/mbgl/storage/resource.cpp
    src/mbgl/storage/resource_transform.cpp
    src/mbgl/storage/response.cpp

    # style
    include/mbgl/style/conversion.hpp
    include/mbgl/style/data_driven_property_value.hpp
    include/mbgl/style/filter.hpp
    include/mbgl/style/filter_evaluator.hpp
    include/mbgl/style/image.hpp
    include/mbgl/style/layer.hpp
    include/mbgl/style/layer_type.hpp
    include/mbgl/style/light.hpp
    include/mbgl/style/position.hpp
    include/mbgl/style/property_value.hpp
    include/mbgl/style/source.hpp
    include/mbgl/style/style.hpp
    include/mbgl/style/transition_options.hpp
    include/mbgl/style/types.hpp
    include/mbgl/style/undefined.hpp
    src/mbgl/style/collection.hpp
    src/mbgl/style/image.cpp
    src/mbgl/style/image_impl.cpp
    src/mbgl/style/image_impl.hpp
    src/mbgl/style/layer.cpp
    src/mbgl/style/layer_impl.cpp
    src/mbgl/style/layer_impl.hpp
    src/mbgl/style/layer_observer.hpp
    src/mbgl/style/layout_property.hpp
    src/mbgl/style/light.cpp
    src/mbgl/style/light_impl.cpp
    src/mbgl/style/light_impl.hpp
    src/mbgl/style/light_observer.hpp
    src/mbgl/style/observer.hpp
    src/mbgl/style/paint_property.hpp
    src/mbgl/style/parser.cpp
    src/mbgl/style/parser.hpp
    src/mbgl/style/properties.hpp
    src/mbgl/style/rapidjson_conversion.hpp
    src/mbgl/style/source.cpp
    src/mbgl/style/source_impl.cpp
    src/mbgl/style/source_impl.hpp
    src/mbgl/style/source_observer.hpp
    src/mbgl/style/style.cpp
    src/mbgl/style/style_impl.cpp
    src/mbgl/style/style_impl.hpp
    src/mbgl/style/types.cpp

    # style/conversion
    include/mbgl/style/conversion/constant.hpp
    include/mbgl/style/conversion/coordinate.hpp
    include/mbgl/style/conversion/data_driven_property_value.hpp
    include/mbgl/style/conversion/filter.hpp
    include/mbgl/style/conversion/function.hpp
    include/mbgl/style/conversion/geojson.hpp
    include/mbgl/style/conversion/geojson_options.hpp
    include/mbgl/style/conversion/layer.hpp
    include/mbgl/style/conversion/light.hpp
    include/mbgl/style/conversion/make_property_setters.hpp
    include/mbgl/style/conversion/position.hpp
    include/mbgl/style/conversion/property_setter.hpp
    include/mbgl/style/conversion/property_value.hpp
    include/mbgl/style/conversion/source.hpp
    include/mbgl/style/conversion/tileset.hpp
    include/mbgl/style/conversion/transition_options.hpp
    src/mbgl/style/conversion/geojson.cpp
    src/mbgl/style/conversion/json.hpp
    src/mbgl/style/conversion/stringify.hpp

    # style/function
    include/mbgl/style/function/camera_function.hpp
    include/mbgl/style/function/categorical_stops.hpp
    include/mbgl/style/function/composite_categorical_stops.hpp
    include/mbgl/style/function/composite_exponential_stops.hpp
    include/mbgl/style/function/composite_function.hpp
    include/mbgl/style/function/composite_interval_stops.hpp
    include/mbgl/style/function/exponential_stops.hpp
    include/mbgl/style/function/identity_stops.hpp
    include/mbgl/style/function/interval_stops.hpp
    include/mbgl/style/function/source_function.hpp
    src/mbgl/style/function/categorical_stops.cpp
    src/mbgl/style/function/identity_stops.cpp

    # style/layers
    include/mbgl/style/layers/background_layer.hpp
    include/mbgl/style/layers/circle_layer.hpp
    include/mbgl/style/layers/custom_layer.hpp
    include/mbgl/style/layers/fill_extrusion_layer.hpp
    include/mbgl/style/layers/fill_layer.hpp
    include/mbgl/style/layers/line_layer.hpp
    include/mbgl/style/layers/raster_layer.hpp
    include/mbgl/style/layers/symbol_layer.hpp
    src/mbgl/style/layers/background_layer.cpp
    src/mbgl/style/layers/background_layer_impl.cpp
    src/mbgl/style/layers/background_layer_impl.hpp
    src/mbgl/style/layers/background_layer_properties.cpp
    src/mbgl/style/layers/background_layer_properties.hpp
    src/mbgl/style/layers/circle_layer.cpp
    src/mbgl/style/layers/circle_layer_impl.cpp
    src/mbgl/style/layers/circle_layer_impl.hpp
    src/mbgl/style/layers/circle_layer_properties.cpp
    src/mbgl/style/layers/circle_layer_properties.hpp
    src/mbgl/style/layers/custom_layer.cpp
    src/mbgl/style/layers/custom_layer_impl.cpp
    src/mbgl/style/layers/custom_layer_impl.hpp
    src/mbgl/style/layers/fill_extrusion_layer.cpp
    src/mbgl/style/layers/fill_extrusion_layer_impl.cpp
    src/mbgl/style/layers/fill_extrusion_layer_impl.hpp
    src/mbgl/style/layers/fill_extrusion_layer_properties.cpp
    src/mbgl/style/layers/fill_extrusion_layer_properties.hpp
    src/mbgl/style/layers/fill_layer.cpp
    src/mbgl/style/layers/fill_layer_impl.cpp
    src/mbgl/style/layers/fill_layer_impl.hpp
    src/mbgl/style/layers/fill_layer_properties.cpp
    src/mbgl/style/layers/fill_layer_properties.hpp
    src/mbgl/style/layers/line_layer.cpp
    src/mbgl/style/layers/line_layer_impl.cpp
    src/mbgl/style/layers/line_layer_impl.hpp
    src/mbgl/style/layers/line_layer_properties.cpp
    src/mbgl/style/layers/line_layer_properties.hpp
    src/mbgl/style/layers/raster_layer.cpp
    src/mbgl/style/layers/raster_layer_impl.cpp
    src/mbgl/style/layers/raster_layer_impl.hpp
    src/mbgl/style/layers/raster_layer_properties.cpp
    src/mbgl/style/layers/raster_layer_properties.hpp
    src/mbgl/style/layers/symbol_layer.cpp
    src/mbgl/style/layers/symbol_layer_impl.cpp
    src/mbgl/style/layers/symbol_layer_impl.hpp
    src/mbgl/style/layers/symbol_layer_properties.cpp
    src/mbgl/style/layers/symbol_layer_properties.hpp

    # style/sources
    include/mbgl/style/sources/geojson_source.hpp
    include/mbgl/style/sources/image_source.hpp
    include/mbgl/style/sources/raster_source.hpp
    include/mbgl/style/sources/vector_source.hpp
    src/mbgl/style/sources/geojson_source.cpp
    src/mbgl/style/sources/geojson_source_impl.cpp
    src/mbgl/style/sources/geojson_source_impl.hpp
    src/mbgl/style/sources/image_source.cpp
    src/mbgl/style/sources/image_source_impl.cpp
    src/mbgl/style/sources/image_source_impl.hpp
    src/mbgl/style/sources/raster_source.cpp
    src/mbgl/style/sources/raster_source_impl.cpp
    src/mbgl/style/sources/raster_source_impl.hpp
    src/mbgl/style/sources/vector_source.cpp
    src/mbgl/style/sources/vector_source_impl.cpp
    src/mbgl/style/sources/vector_source_impl.hpp

    # text
    src/mbgl/text/bidi.hpp
    src/mbgl/text/check_max_angle.cpp
    src/mbgl/text/check_max_angle.hpp
    src/mbgl/text/collision_feature.cpp
    src/mbgl/text/collision_feature.hpp
    src/mbgl/text/collision_tile.cpp
    src/mbgl/text/collision_tile.hpp
    src/mbgl/text/get_anchors.cpp
    src/mbgl/text/get_anchors.hpp
    src/mbgl/text/glyph.cpp
    src/mbgl/text/glyph.hpp
    src/mbgl/text/glyph_atlas.cpp
    src/mbgl/text/glyph_atlas.hpp
    src/mbgl/text/glyph_manager.cpp
    src/mbgl/text/glyph_manager.hpp
    src/mbgl/text/glyph_manager_observer.hpp
    src/mbgl/text/glyph_pbf.cpp
    src/mbgl/text/glyph_pbf.hpp
    src/mbgl/text/glyph_range.hpp
    src/mbgl/text/placement_config.hpp
    src/mbgl/text/quads.cpp
    src/mbgl/text/quads.hpp
    src/mbgl/text/shaping.cpp
    src/mbgl/text/shaping.hpp

    # tile
    include/mbgl/tile/tile_id.hpp
    src/mbgl/tile/geojson_tile.cpp
    src/mbgl/tile/geojson_tile.hpp
    src/mbgl/tile/geometry_tile.cpp
    src/mbgl/tile/geometry_tile.hpp
    src/mbgl/tile/geometry_tile_data.cpp
    src/mbgl/tile/geometry_tile_data.hpp
    src/mbgl/tile/geometry_tile_worker.cpp
    src/mbgl/tile/geometry_tile_worker.hpp
    src/mbgl/tile/raster_tile.cpp
    src/mbgl/tile/raster_tile.hpp
    src/mbgl/tile/raster_tile_worker.cpp
    src/mbgl/tile/raster_tile_worker.hpp
    src/mbgl/tile/tile.cpp
    src/mbgl/tile/tile.hpp
    src/mbgl/tile/tile_cache.cpp
    src/mbgl/tile/tile_cache.hpp
    src/mbgl/tile/tile_id_hash.cpp
    src/mbgl/tile/tile_id_io.cpp
    src/mbgl/tile/tile_loader.hpp
    src/mbgl/tile/tile_loader_impl.hpp
    src/mbgl/tile/tile_observer.hpp
    src/mbgl/tile/vector_tile.cpp
    src/mbgl/tile/vector_tile.hpp
    src/mbgl/tile/vector_tile_data.cpp
    src/mbgl/tile/vector_tile_data.hpp

    # util
    include/mbgl/util/any.hpp
    include/mbgl/util/async_request.hpp
    include/mbgl/util/async_task.hpp
    include/mbgl/util/char_array_buffer.hpp
    include/mbgl/util/chrono.hpp
    include/mbgl/util/color.hpp
    include/mbgl/util/compression.hpp
    include/mbgl/util/constants.hpp
    include/mbgl/util/convert.hpp
    include/mbgl/util/enum.hpp
    include/mbgl/util/event.hpp
    include/mbgl/util/exception.hpp
    include/mbgl/util/feature.hpp
    include/mbgl/util/font_stack.hpp
    include/mbgl/util/geo.hpp
    include/mbgl/util/geojson.hpp
    include/mbgl/util/geometry.hpp
    include/mbgl/util/ignore.hpp
    include/mbgl/util/image.hpp
    include/mbgl/util/immutable.hpp
    include/mbgl/util/indexed_tuple.hpp
    include/mbgl/util/interpolate.hpp
    include/mbgl/util/logging.hpp
    include/mbgl/util/noncopyable.hpp
    include/mbgl/util/optional.hpp
    include/mbgl/util/platform.hpp
    include/mbgl/util/premultiply.hpp
    include/mbgl/util/projection.hpp
    include/mbgl/util/range.hpp
    include/mbgl/util/run_loop.hpp
    include/mbgl/util/size.hpp
    include/mbgl/util/string.hpp
    include/mbgl/util/thread.hpp
    include/mbgl/util/tileset.hpp
    include/mbgl/util/timer.hpp
    include/mbgl/util/traits.hpp
    include/mbgl/util/type_list.hpp
    include/mbgl/util/unitbezier.hpp
    include/mbgl/util/util.hpp
    include/mbgl/util/variant.hpp
    include/mbgl/util/work_request.hpp
    include/mbgl/util/work_task.hpp
    include/mbgl/util/work_task_impl.hpp
    src/mbgl/util/chrono.cpp
    src/mbgl/util/clip_id.cpp
    src/mbgl/util/clip_id.hpp
    src/mbgl/util/color.cpp
    src/mbgl/util/compression.cpp
    src/mbgl/util/constants.cpp
    src/mbgl/util/convert.cpp
    src/mbgl/util/dtoa.cpp
    src/mbgl/util/dtoa.hpp
    src/mbgl/util/event.cpp
    src/mbgl/util/font_stack.cpp
    src/mbgl/util/geo.cpp
    src/mbgl/util/geojson_impl.cpp
    src/mbgl/util/grid_index.cpp
    src/mbgl/util/grid_index.hpp
    src/mbgl/util/http_header.cpp
    src/mbgl/util/http_header.hpp
    src/mbgl/util/http_timeout.cpp
    src/mbgl/util/http_timeout.hpp
    src/mbgl/util/i18n.cpp
    src/mbgl/util/i18n.hpp
    src/mbgl/util/interpolate.cpp
    src/mbgl/util/intersection_tests.cpp
    src/mbgl/util/intersection_tests.hpp
    src/mbgl/util/io.cpp
    src/mbgl/util/io.hpp
    src/mbgl/util/logging.cpp
    src/mbgl/util/longest_common_subsequence.hpp
    src/mbgl/util/mapbox.cpp
    src/mbgl/util/mapbox.hpp
    src/mbgl/util/mat2.cpp
    src/mbgl/util/mat2.hpp
    src/mbgl/util/mat3.cpp
    src/mbgl/util/mat3.hpp
    src/mbgl/util/mat4.cpp
    src/mbgl/util/mat4.hpp
    src/mbgl/util/math.hpp
    src/mbgl/util/offscreen_texture.cpp
    src/mbgl/util/offscreen_texture.hpp
    src/mbgl/util/premultiply.cpp
    src/mbgl/util/rapidjson.hpp
    src/mbgl/util/rect.hpp
    src/mbgl/util/std.hpp
    src/mbgl/util/stopwatch.cpp
    src/mbgl/util/stopwatch.hpp
    src/mbgl/util/string.cpp
    src/mbgl/util/thread_local.hpp
    src/mbgl/util/throttler.cpp
    src/mbgl/util/throttler.hpp
    src/mbgl/util/tile_coordinate.hpp
    src/mbgl/util/tile_cover.cpp
    src/mbgl/util/tile_cover.hpp
    src/mbgl/util/token.hpp
    src/mbgl/util/url.cpp
    src/mbgl/util/url.hpp
    src/mbgl/util/utf.hpp
    src/mbgl/util/version.cpp
    src/mbgl/util/version.hpp
    src/mbgl/util/work_request.cpp
)
