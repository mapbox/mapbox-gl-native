# Do not edit. Regenerate this with ./scripts/generate-core-files.sh

set(MBGL_CORE_FILES
    # algorithm
    src/mbgl/algorithm/covered_by_children.hpp
    src/mbgl/algorithm/generate_clip_ids.cpp
    src/mbgl/algorithm/generate_clip_ids.hpp
    src/mbgl/algorithm/generate_clip_ids_impl.hpp
    src/mbgl/algorithm/update_renderables.hpp

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
    src/mbgl/annotation/shape_annotation_impl.cpp
    src/mbgl/annotation/shape_annotation_impl.hpp
    src/mbgl/annotation/style_sourced_annotation_impl.cpp
    src/mbgl/annotation/style_sourced_annotation_impl.hpp
    src/mbgl/annotation/symbol_annotation_impl.cpp
    src/mbgl/annotation/symbol_annotation_impl.hpp

    # clipper
    src/clipper/clipper.cpp
    src/clipper/clipper.hpp

    # csscolorparser
    src/csscolorparser/csscolorparser.cpp
    src/csscolorparser/csscolorparser.hpp

    # geometry
    src/mbgl/geometry/anchor.hpp
    src/mbgl/geometry/binpack.hpp
    src/mbgl/geometry/buffer.hpp
    src/mbgl/geometry/circle_buffer.cpp
    src/mbgl/geometry/circle_buffer.hpp
    src/mbgl/geometry/collision_box_buffer.cpp
    src/mbgl/geometry/collision_box_buffer.hpp
    src/mbgl/geometry/debug_font_buffer.cpp
    src/mbgl/geometry/debug_font_buffer.hpp
    src/mbgl/geometry/debug_font_data.hpp
    src/mbgl/geometry/elements_buffer.cpp
    src/mbgl/geometry/elements_buffer.hpp
    src/mbgl/geometry/feature_index.cpp
    src/mbgl/geometry/feature_index.hpp
    src/mbgl/geometry/fill_buffer.cpp
    src/mbgl/geometry/fill_buffer.hpp
    src/mbgl/geometry/glyph_atlas.cpp
    src/mbgl/geometry/glyph_atlas.hpp
    src/mbgl/geometry/icon_buffer.cpp
    src/mbgl/geometry/icon_buffer.hpp
    src/mbgl/geometry/line_atlas.cpp
    src/mbgl/geometry/line_atlas.hpp
    src/mbgl/geometry/line_buffer.cpp
    src/mbgl/geometry/line_buffer.hpp
    src/mbgl/geometry/static_vertex_buffer.cpp
    src/mbgl/geometry/static_vertex_buffer.hpp
    src/mbgl/geometry/text_buffer.cpp
    src/mbgl/geometry/text_buffer.hpp
    src/mbgl/geometry/vao.cpp
    src/mbgl/geometry/vao.hpp

    # gl
    include/mbgl/gl/gl.hpp
    include/mbgl/gl/gl_helper.hpp
    include/mbgl/gl/gl_values.hpp
    src/mbgl/gl/debugging.cpp
    src/mbgl/gl/debugging.hpp
    src/mbgl/gl/gl.cpp
    src/mbgl/gl/gl_config.cpp
    src/mbgl/gl/gl_config.hpp
    src/mbgl/gl/object_store.cpp
    src/mbgl/gl/object_store.hpp

    # map
    include/mbgl/map/camera.hpp
    include/mbgl/map/map.hpp
    include/mbgl/map/mode.hpp
    include/mbgl/map/update.hpp
    include/mbgl/map/view.hpp
    src/mbgl/map/change.hpp
    src/mbgl/map/map.cpp
    src/mbgl/map/transform.cpp
    src/mbgl/map/transform.hpp
    src/mbgl/map/transform_state.cpp
    src/mbgl/map/transform_state.hpp
    src/mbgl/map/view.cpp
    src/mbgl/map/zoom_history.hpp

    # math
    include/mbgl/math/clamp.hpp
    include/mbgl/math/minmax.hpp
    include/mbgl/math/wrap.hpp

    # mbgl
    include/mbgl/mbgl.hpp

    # parsedate
    src/parsedate/parsedate.c
    src/parsedate/parsedate.h

    # platform
    include/mbgl/platform/event.hpp
    include/mbgl/platform/log.hpp
    include/mbgl/platform/platform.hpp
    src/mbgl/platform/event.cpp
    src/mbgl/platform/log.cpp

    # platform/darwin
    include/mbgl/platform/darwin/reachability.h

    # platform/default
    include/mbgl/platform/default/glfw_view.hpp
    include/mbgl/platform/default/headless_display.hpp
    include/mbgl/platform/default/headless_view.hpp
    include/mbgl/platform/default/settings_json.hpp

    # renderer
    src/mbgl/renderer/bucket.hpp
    src/mbgl/renderer/circle_bucket.cpp
    src/mbgl/renderer/circle_bucket.hpp
    src/mbgl/renderer/debug_bucket.cpp
    src/mbgl/renderer/debug_bucket.hpp
    src/mbgl/renderer/fill_bucket.cpp
    src/mbgl/renderer/fill_bucket.hpp
    src/mbgl/renderer/frame_history.cpp
    src/mbgl/renderer/frame_history.hpp
    src/mbgl/renderer/line_bucket.cpp
    src/mbgl/renderer/line_bucket.hpp
    src/mbgl/renderer/paint_parameters.hpp
    src/mbgl/renderer/painter.cpp
    src/mbgl/renderer/painter.hpp
    src/mbgl/renderer/painter_background.cpp
    src/mbgl/renderer/painter_circle.cpp
    src/mbgl/renderer/painter_clipping.cpp
    src/mbgl/renderer/painter_debug.cpp
    src/mbgl/renderer/painter_fill.cpp
    src/mbgl/renderer/painter_line.cpp
    src/mbgl/renderer/painter_raster.cpp
    src/mbgl/renderer/painter_symbol.cpp
    src/mbgl/renderer/raster_bucket.cpp
    src/mbgl/renderer/raster_bucket.hpp
    src/mbgl/renderer/render_item.hpp
    src/mbgl/renderer/render_pass.hpp
    src/mbgl/renderer/render_tile.cpp
    src/mbgl/renderer/render_tile.hpp
    src/mbgl/renderer/symbol_bucket.cpp
    src/mbgl/renderer/symbol_bucket.hpp

    # shader
    src/mbgl/shader/circle_shader.cpp
    src/mbgl/shader/circle_shader.hpp
    src/mbgl/shader/collision_box_shader.cpp
    src/mbgl/shader/collision_box_shader.hpp
    src/mbgl/shader/icon_shader.cpp
    src/mbgl/shader/icon_shader.hpp
    src/mbgl/shader/line_shader.cpp
    src/mbgl/shader/line_shader.hpp
    src/mbgl/shader/linepattern_shader.cpp
    src/mbgl/shader/linepattern_shader.hpp
    src/mbgl/shader/linesdf_shader.cpp
    src/mbgl/shader/linesdf_shader.hpp
    src/mbgl/shader/outline_shader.cpp
    src/mbgl/shader/outline_shader.hpp
    src/mbgl/shader/outlinepattern_shader.cpp
    src/mbgl/shader/outlinepattern_shader.hpp
    src/mbgl/shader/pattern_shader.cpp
    src/mbgl/shader/pattern_shader.hpp
    src/mbgl/shader/plain_shader.cpp
    src/mbgl/shader/plain_shader.hpp
    src/mbgl/shader/raster_shader.cpp
    src/mbgl/shader/raster_shader.hpp
    src/mbgl/shader/sdf_shader.cpp
    src/mbgl/shader/sdf_shader.hpp
    src/mbgl/shader/shader.cpp
    src/mbgl/shader/shader.hpp
    src/mbgl/shader/shaders.hpp
    src/mbgl/shader/uniform.cpp
    src/mbgl/shader/uniform.hpp

    # sprite
    include/mbgl/sprite/sprite_image.hpp
    src/mbgl/sprite/sprite_atlas.cpp
    src/mbgl/sprite/sprite_atlas.hpp
    src/mbgl/sprite/sprite_image.cpp
    src/mbgl/sprite/sprite_parser.cpp
    src/mbgl/sprite/sprite_parser.hpp
    src/mbgl/sprite/sprite_store.cpp
    src/mbgl/sprite/sprite_store.hpp
    src/mbgl/sprite/sprite_store_observer.hpp

    # storage
    include/mbgl/storage/default_file_source.hpp
    include/mbgl/storage/file_source.hpp
    include/mbgl/storage/network_status.hpp
    include/mbgl/storage/offline.hpp
    include/mbgl/storage/online_file_source.hpp
    include/mbgl/storage/resource.hpp
    include/mbgl/storage/response.hpp
    src/mbgl/storage/asset_file_source.hpp
    src/mbgl/storage/http_file_source.hpp
    src/mbgl/storage/network_status.cpp
    src/mbgl/storage/resource.cpp
    src/mbgl/storage/response.cpp

    # style
    include/mbgl/style/conversion.hpp
    include/mbgl/style/filter.hpp
    include/mbgl/style/filter_evaluator.hpp
    include/mbgl/style/function.hpp
    include/mbgl/style/layer.hpp
    include/mbgl/style/property_value.hpp
    include/mbgl/style/source.hpp
    include/mbgl/style/transition_options.hpp
    include/mbgl/style/types.hpp
    src/mbgl/style/bucket_parameters.cpp
    src/mbgl/style/bucket_parameters.hpp
    src/mbgl/style/calculation_parameters.hpp
    src/mbgl/style/cascade_parameters.hpp
    src/mbgl/style/class_dictionary.cpp
    src/mbgl/style/class_dictionary.hpp
    src/mbgl/style/layer.cpp
    src/mbgl/style/layer_impl.cpp
    src/mbgl/style/layer_impl.hpp
    src/mbgl/style/layout_property.hpp
    src/mbgl/style/observer.hpp
    src/mbgl/style/paint_property.hpp
    src/mbgl/style/parser.cpp
    src/mbgl/style/parser.hpp
    src/mbgl/style/property_evaluator.cpp
    src/mbgl/style/property_evaluator.hpp
    src/mbgl/style/property_parsing.cpp
    src/mbgl/style/property_parsing.hpp
    src/mbgl/style/query_parameters.hpp
    src/mbgl/style/rapidjson_conversion.hpp
    src/mbgl/style/source.cpp
    src/mbgl/style/source_impl.cpp
    src/mbgl/style/source_impl.hpp
    src/mbgl/style/source_observer.hpp
    src/mbgl/style/style.cpp
    src/mbgl/style/style.hpp
    src/mbgl/style/tile_source_impl.cpp
    src/mbgl/style/tile_source_impl.hpp
    src/mbgl/style/types.cpp
    src/mbgl/style/update_parameters.hpp

    # style/conversion
    include/mbgl/style/conversion/constant.hpp
    include/mbgl/style/conversion/filter.hpp
    include/mbgl/style/conversion/function.hpp
    include/mbgl/style/conversion/geojson.hpp
    include/mbgl/style/conversion/layer.hpp
    include/mbgl/style/conversion/make_property_setters.hpp
    include/mbgl/style/conversion/property_setter.hpp
    include/mbgl/style/conversion/property_value.hpp
    include/mbgl/style/conversion/source.hpp
    include/mbgl/style/conversion/tileset.hpp

    # style/layers
    include/mbgl/style/layers/background_layer.hpp
    include/mbgl/style/layers/circle_layer.hpp
    include/mbgl/style/layers/custom_layer.hpp
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
    include/mbgl/style/sources/raster_source.hpp
    include/mbgl/style/sources/vector_source.hpp
    src/mbgl/style/sources/geojson_source.cpp
    src/mbgl/style/sources/geojson_source_impl.cpp
    src/mbgl/style/sources/geojson_source_impl.hpp
    src/mbgl/style/sources/raster_source.cpp
    src/mbgl/style/sources/raster_source_impl.cpp
    src/mbgl/style/sources/raster_source_impl.hpp
    src/mbgl/style/sources/vector_source.cpp
    src/mbgl/style/sources/vector_source_impl.cpp
    src/mbgl/style/sources/vector_source_impl.hpp

    # text
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
    src/mbgl/text/glyph_pbf.cpp
    src/mbgl/text/glyph_pbf.hpp
    src/mbgl/text/glyph_range.hpp
    src/mbgl/text/glyph_set.cpp
    src/mbgl/text/glyph_set.hpp
    src/mbgl/text/glyph_store.cpp
    src/mbgl/text/glyph_store.hpp
    src/mbgl/text/glyph_store_observer.hpp
    src/mbgl/text/placement_config.hpp
    src/mbgl/text/quads.cpp
    src/mbgl/text/quads.hpp
    src/mbgl/text/shaping.cpp
    src/mbgl/text/shaping.hpp

    # tile
    src/mbgl/tile/geojson_tile.cpp
    src/mbgl/tile/geojson_tile.hpp
    src/mbgl/tile/geometry_tile.cpp
    src/mbgl/tile/geometry_tile.hpp
    src/mbgl/tile/geometry_tile_data.cpp
    src/mbgl/tile/geometry_tile_data.hpp
    src/mbgl/tile/raster_tile.cpp
    src/mbgl/tile/raster_tile.hpp
    src/mbgl/tile/tile.cpp
    src/mbgl/tile/tile.hpp
    src/mbgl/tile/tile_cache.cpp
    src/mbgl/tile/tile_cache.hpp
    src/mbgl/tile/tile_id.hpp
    src/mbgl/tile/tile_id_io.cpp
    src/mbgl/tile/tile_loader.hpp
    src/mbgl/tile/tile_loader_impl.hpp
    src/mbgl/tile/tile_observer.hpp
    src/mbgl/tile/tile_worker.cpp
    src/mbgl/tile/tile_worker.hpp
    src/mbgl/tile/vector_tile.cpp
    src/mbgl/tile/vector_tile.hpp

    # util
    include/mbgl/util/async_request.hpp
    include/mbgl/util/char_array_buffer.hpp
    include/mbgl/util/chrono.hpp
    include/mbgl/util/color.hpp
    include/mbgl/util/compression.hpp
    include/mbgl/util/constants.hpp
    include/mbgl/util/convert.hpp
    include/mbgl/util/default_styles.hpp
    include/mbgl/util/enum.hpp
    include/mbgl/util/exception.hpp
    include/mbgl/util/feature.hpp
    include/mbgl/util/font_stack.hpp
    include/mbgl/util/geo.hpp
    include/mbgl/util/geojson.hpp
    include/mbgl/util/geometry.hpp
    include/mbgl/util/image.hpp
    include/mbgl/util/noncopyable.hpp
    include/mbgl/util/optional.hpp
    include/mbgl/util/projection.hpp
    include/mbgl/util/range.hpp
    include/mbgl/util/run_loop.hpp
    include/mbgl/util/string.hpp
    include/mbgl/util/tileset.hpp
    include/mbgl/util/timer.hpp
    include/mbgl/util/traits.hpp
    include/mbgl/util/unitbezier.hpp
    include/mbgl/util/util.hpp
    include/mbgl/util/variant.hpp
    include/mbgl/util/work_request.hpp
    include/mbgl/util/work_task.hpp
    src/mbgl/util/async_task.hpp
    src/mbgl/util/chrono.cpp
    src/mbgl/util/clip_id.cpp
    src/mbgl/util/clip_id.hpp
    src/mbgl/util/clip_lines.cpp
    src/mbgl/util/clip_lines.hpp
    src/mbgl/util/color.cpp
    src/mbgl/util/compression.cpp
    src/mbgl/util/constants.cpp
    src/mbgl/util/convert.cpp
    src/mbgl/util/default_styles.cpp
    src/mbgl/util/dtoa.cpp
    src/mbgl/util/dtoa.hpp
    src/mbgl/util/exclusive.hpp
    src/mbgl/util/font_stack.cpp
    src/mbgl/util/geo.cpp
    src/mbgl/util/grid_index.cpp
    src/mbgl/util/grid_index.hpp
    src/mbgl/util/http_header.cpp
    src/mbgl/util/http_header.hpp
    src/mbgl/util/interpolate.hpp
    src/mbgl/util/intersection_tests.cpp
    src/mbgl/util/intersection_tests.hpp
    src/mbgl/util/io.cpp
    src/mbgl/util/io.hpp
    src/mbgl/util/mapbox.cpp
    src/mbgl/util/mapbox.hpp
    src/mbgl/util/mat2.cpp
    src/mbgl/util/mat2.hpp
    src/mbgl/util/mat3.cpp
    src/mbgl/util/mat3.hpp
    src/mbgl/util/mat4.cpp
    src/mbgl/util/mat4.hpp
    src/mbgl/util/math.cpp
    src/mbgl/util/math.hpp
    src/mbgl/util/merge_lines.cpp
    src/mbgl/util/merge_lines.hpp
    src/mbgl/util/premultiply.cpp
    src/mbgl/util/premultiply.hpp
    src/mbgl/util/ptr.hpp
    src/mbgl/util/rapidjson.hpp
    src/mbgl/util/raster.cpp
    src/mbgl/util/raster.hpp
    src/mbgl/util/rect.hpp
    src/mbgl/util/std.hpp
    src/mbgl/util/stopwatch.cpp
    src/mbgl/util/stopwatch.hpp
    src/mbgl/util/thread.hpp
    src/mbgl/util/thread_context.cpp
    src/mbgl/util/thread_context.hpp
    src/mbgl/util/thread_local.hpp
    src/mbgl/util/tile_coordinate.hpp
    src/mbgl/util/tile_cover.cpp
    src/mbgl/util/tile_cover.hpp
    src/mbgl/util/token.hpp
    src/mbgl/util/url.cpp
    src/mbgl/util/url.hpp
    src/mbgl/util/utf.hpp
    src/mbgl/util/version_info.cpp
    src/mbgl/util/work_queue.cpp
    src/mbgl/util/work_queue.hpp
    src/mbgl/util/work_request.cpp
    src/mbgl/util/worker.cpp
    src/mbgl/util/worker.hpp
)
