TARGET = qmapboxgl

load(qt_helper_lib)

CONFIG += qt c++14 exceptions warn_off staticlib object_parallel_to_source
CONFIG -= c++1z

QT += network-private \
      gui-private \
      sql-private

QMAKE_CXXFLAGS += \
    -DNDEBUG \
    -DQT_IMAGE_DECODERS \
    -DQT_BUILD_MAPBOXGL_LIB \
    -DRAPIDJSON_HAS_STDSTRING=1 \
    -DMBGL_USE_GLES2 \
    -D__QT__ \
    -O3 \
    -ftemplate-depth=1024 \
    -fvisibility-inlines-hidden \
    -fvisibility=hidden

android|win32|darwin|qnx {
    SOURCES += \
        platform/qt/src/thread.cpp
} else {
    SOURCES += \
        platform/default/thread.cpp
}

android|gcc {
    QMAKE_CFLAGS += -std=c99
}

darwin {
    LIBS += \
        -framework Foundation
}

win32 {
    QMAKE_CXXFLAGS += \
        -D_WINDOWS \
        -DNOGDI \
        -DNOMINMAX \
        -D_USE_MATH_DEFINES
}

qtConfig(system-zlib) {
    QMAKE_USE_PRIVATE += zlib
} else {
    QT_PRIVATE += zlib-private
}

# QTBUG-59035
TR_EXCLUDE += $$PWD/*

qtConfig(icu) {
    QMAKE_USE_PRIVATE += icu

    SOURCES += \
        platform/default/bidi.cpp
} else {
    SOURCES += \
        platform/qt/src/bidi.cpp
}

SOURCES += \
    platform/qt/src/async_task.cpp \
    platform/qt/src/http_file_source.cpp \
    platform/qt/src/http_request.cpp \
    platform/qt/src/qmapbox.cpp \
    platform/qt/src/qmapboxgl.cpp \
    platform/qt/src/qmapboxgl_map_observer.cpp \
    platform/qt/src/qmapboxgl_map_renderer.cpp \
    platform/qt/src/qmapboxgl_renderer_backend.cpp \
    platform/qt/src/qmapboxgl_scheduler.cpp \
    platform/qt/src/qt_geojson.cpp \
    platform/qt/src/qt_image.cpp \
    platform/qt/src/qt_logging.cpp \
    platform/qt/src/run_loop.cpp \
    platform/qt/src/sqlite3.cpp \
    platform/qt/src/string_stdlib.cpp \
    platform/qt/src/thread_local.cpp \
    platform/qt/src/timer.cpp \
    platform/qt/src/utf.cpp \
    src/csscolorparser/csscolorparser.cpp \
    src/mbgl/actor/mailbox.cpp \
    src/mbgl/actor/scheduler.cpp \
    src/mbgl/algorithm/generate_clip_ids.cpp \
    src/mbgl/annotation/annotation_manager.cpp \
    src/mbgl/annotation/annotation_source.cpp \
    src/mbgl/annotation/annotation_tile.cpp \
    src/mbgl/annotation/fill_annotation_impl.cpp \
    src/mbgl/annotation/line_annotation_impl.cpp \
    src/mbgl/annotation/render_annotation_source.cpp \
    src/mbgl/annotation/shape_annotation_impl.cpp \
    src/mbgl/annotation/symbol_annotation_impl.cpp \
    src/mbgl/geometry/dem_data.cpp \
    src/mbgl/geometry/feature_index.cpp \
    src/mbgl/geometry/line_atlas.cpp \
    src/mbgl/gl/attribute.cpp \
    src/mbgl/gl/color_mode.cpp \
    src/mbgl/gl/context.cpp \
    src/mbgl/gl/debugging.cpp \
    src/mbgl/gl/debugging_extension.cpp \
    src/mbgl/gl/depth_mode.cpp \
    src/mbgl/gl/gl.cpp \
    src/mbgl/gl/object.cpp \
    src/mbgl/gl/stencil_mode.cpp \
    src/mbgl/gl/uniform.cpp \
    src/mbgl/gl/value.cpp \
    src/mbgl/gl/vertex_array.cpp \
    src/mbgl/layout/clip_lines.cpp \
    src/mbgl/layout/merge_lines.cpp \
    src/mbgl/layout/symbol_instance.cpp \
    src/mbgl/layout/symbol_layout.cpp \
    src/mbgl/layout/symbol_projection.cpp \
    src/mbgl/map/map.cpp \
    src/mbgl/map/transform.cpp \
    src/mbgl/map/transform_state.cpp \
    src/mbgl/math/log2.cpp \
    src/mbgl/programs/background_program.cpp \
    src/mbgl/programs/binary_program.cpp \
    src/mbgl/programs/circle_program.cpp \
    src/mbgl/programs/collision_box_program.cpp \
    src/mbgl/programs/extrusion_texture_program.cpp \
    src/mbgl/programs/fill_extrusion_program.cpp \
    src/mbgl/programs/fill_program.cpp \
    src/mbgl/programs/heatmap_program.cpp \
    src/mbgl/programs/heatmap_texture_program.cpp \
    src/mbgl/programs/hillshade_prepare_program.cpp \
    src/mbgl/programs/hillshade_program.cpp \
    src/mbgl/programs/line_program.cpp \
    src/mbgl/programs/program_parameters.cpp \
    src/mbgl/programs/raster_program.cpp \
    src/mbgl/programs/symbol_program.cpp \
    src/mbgl/renderer/backend_scope.cpp \
    src/mbgl/renderer/bucket_parameters.cpp \
    src/mbgl/renderer/buckets/circle_bucket.cpp \
    src/mbgl/renderer/buckets/debug_bucket.cpp \
    src/mbgl/renderer/buckets/fill_bucket.cpp \
    src/mbgl/renderer/buckets/fill_extrusion_bucket.cpp \
    src/mbgl/renderer/buckets/heatmap_bucket.cpp \
    src/mbgl/renderer/buckets/hillshade_bucket.cpp \
    src/mbgl/renderer/buckets/line_bucket.cpp \
    src/mbgl/renderer/buckets/raster_bucket.cpp \
    src/mbgl/renderer/buckets/symbol_bucket.cpp \
    src/mbgl/renderer/cross_faded_property_evaluator.cpp \
    src/mbgl/renderer/group_by_layout.cpp \
    src/mbgl/renderer/image_atlas.cpp \
    src/mbgl/renderer/image_manager.cpp \
    src/mbgl/renderer/layers/render_background_layer.cpp \
    src/mbgl/renderer/layers/render_circle_layer.cpp \
    src/mbgl/renderer/layers/render_custom_layer.cpp \
    src/mbgl/renderer/layers/render_fill_extrusion_layer.cpp \
    src/mbgl/renderer/layers/render_fill_layer.cpp \
    src/mbgl/renderer/layers/render_heatmap_layer.cpp \
    src/mbgl/renderer/layers/render_hillshade_layer.cpp \
    src/mbgl/renderer/layers/render_line_layer.cpp \
    src/mbgl/renderer/layers/render_raster_layer.cpp \
    src/mbgl/renderer/layers/render_symbol_layer.cpp \
    src/mbgl/renderer/paint_parameters.cpp \
    src/mbgl/renderer/render_layer.cpp \
    src/mbgl/renderer/render_light.cpp \
    src/mbgl/renderer/render_source.cpp \
    src/mbgl/renderer/render_static_data.cpp \
    src/mbgl/renderer/render_tile.cpp \
    src/mbgl/renderer/renderer.cpp \
    src/mbgl/renderer/renderer_backend.cpp \
    src/mbgl/renderer/renderer_impl.cpp \
    src/mbgl/renderer/sources/render_custom_geometry_source.cpp \
    src/mbgl/renderer/sources/render_geojson_source.cpp \
    src/mbgl/renderer/sources/render_image_source.cpp \
    src/mbgl/renderer/sources/render_raster_dem_source.cpp \
    src/mbgl/renderer/sources/render_raster_source.cpp \
    src/mbgl/renderer/sources/render_vector_source.cpp \
    src/mbgl/renderer/style_diff.cpp \
    src/mbgl/renderer/tile_pyramid.cpp \
    src/mbgl/shaders/background.cpp \
    src/mbgl/shaders/background_pattern.cpp \
    src/mbgl/shaders/circle.cpp \
    src/mbgl/shaders/clipping_mask.cpp \
    src/mbgl/shaders/collision_box.cpp \
    src/mbgl/shaders/collision_circle.cpp \
    src/mbgl/shaders/debug.cpp \
    src/mbgl/shaders/extrusion_texture.cpp \
    src/mbgl/shaders/fill.cpp \
    src/mbgl/shaders/fill_extrusion.cpp \
    src/mbgl/shaders/fill_extrusion_pattern.cpp \
    src/mbgl/shaders/fill_outline.cpp \
    src/mbgl/shaders/fill_outline_pattern.cpp \
    src/mbgl/shaders/fill_pattern.cpp \
    src/mbgl/shaders/heatmap.cpp \
    src/mbgl/shaders/heatmap_texture.cpp \
    src/mbgl/shaders/hillshade.cpp \
    src/mbgl/shaders/hillshade_prepare.cpp \
    src/mbgl/shaders/line.cpp \
    src/mbgl/shaders/line_pattern.cpp \
    src/mbgl/shaders/line_sdf.cpp \
    src/mbgl/shaders/preludes.cpp \
    src/mbgl/shaders/raster.cpp \
    src/mbgl/shaders/shaders.cpp \
    src/mbgl/shaders/source.cpp \
    src/mbgl/shaders/symbol_icon.cpp \
    src/mbgl/shaders/symbol_sdf.cpp \
    src/mbgl/sprite/sprite_loader.cpp \
    src/mbgl/sprite/sprite_loader_worker.cpp \
    src/mbgl/sprite/sprite_parser.cpp \
    src/mbgl/storage/network_status.cpp \
    src/mbgl/storage/resource.cpp \
    src/mbgl/storage/resource_transform.cpp \
    src/mbgl/storage/response.cpp \
    src/mbgl/style/conversion/constant.cpp \
    src/mbgl/style/conversion/coordinate.cpp \
    src/mbgl/style/conversion/filter.cpp \
    src/mbgl/style/conversion/function.cpp \
    src/mbgl/style/conversion/geojson.cpp \
    src/mbgl/style/conversion/geojson_options.cpp \
    src/mbgl/style/conversion/get_json_type.cpp \
    src/mbgl/style/conversion/layer.cpp \
    src/mbgl/style/conversion/light.cpp \
    src/mbgl/style/conversion/position.cpp \
    src/mbgl/style/conversion/source.cpp \
    src/mbgl/style/conversion/tileset.cpp \
    src/mbgl/style/conversion/transition_options.cpp \
    src/mbgl/style/custom_tile_loader.cpp \
    src/mbgl/style/expression/array_assertion.cpp \
    src/mbgl/style/expression/assertion.cpp \
    src/mbgl/style/expression/at.cpp \
    src/mbgl/style/expression/boolean_operator.cpp \
    src/mbgl/style/expression/case.cpp \
    src/mbgl/style/expression/check_subtype.cpp \
    src/mbgl/style/expression/coalesce.cpp \
    src/mbgl/style/expression/coercion.cpp \
    src/mbgl/style/expression/collator_expression.cpp \
    src/mbgl/style/expression/compound_expression.cpp \
    src/mbgl/style/expression/dsl.cpp \
    src/mbgl/style/expression/equals.cpp \
    src/mbgl/style/expression/expression.cpp \
    src/mbgl/style/expression/find_zoom_curve.cpp \
    src/mbgl/style/expression/get_covering_stops.cpp \
    src/mbgl/style/expression/interpolate.cpp \
    src/mbgl/style/expression/is_constant.cpp \
    src/mbgl/style/expression/is_expression.cpp \
    src/mbgl/style/expression/length.cpp \
    src/mbgl/style/expression/let.cpp \
    src/mbgl/style/expression/literal.cpp \
    src/mbgl/style/expression/match.cpp \
    src/mbgl/style/expression/parsing_context.cpp \
    src/mbgl/style/expression/step.cpp \
    src/mbgl/style/expression/util.cpp \
    src/mbgl/style/expression/value.cpp \
    src/mbgl/style/filter.cpp \
    src/mbgl/style/image.cpp \
    src/mbgl/style/image_impl.cpp \
    src/mbgl/style/layer.cpp \
    src/mbgl/style/layer_impl.cpp \
    src/mbgl/style/layers/background_layer.cpp \
    src/mbgl/style/layers/background_layer_impl.cpp \
    src/mbgl/style/layers/background_layer_properties.cpp \
    src/mbgl/style/layers/circle_layer.cpp \
    src/mbgl/style/layers/circle_layer_impl.cpp \
    src/mbgl/style/layers/circle_layer_properties.cpp \
    src/mbgl/style/layers/custom_layer.cpp \
    src/mbgl/style/layers/custom_layer_impl.cpp \
    src/mbgl/style/layers/fill_extrusion_layer.cpp \
    src/mbgl/style/layers/fill_extrusion_layer_impl.cpp \
    src/mbgl/style/layers/fill_extrusion_layer_properties.cpp \
    src/mbgl/style/layers/fill_layer.cpp \
    src/mbgl/style/layers/fill_layer_impl.cpp \
    src/mbgl/style/layers/fill_layer_properties.cpp \
    src/mbgl/style/layers/heatmap_layer.cpp \
    src/mbgl/style/layers/heatmap_layer_impl.cpp \
    src/mbgl/style/layers/heatmap_layer_properties.cpp \
    src/mbgl/style/layers/hillshade_layer.cpp \
    src/mbgl/style/layers/hillshade_layer_impl.cpp \
    src/mbgl/style/layers/hillshade_layer_properties.cpp \
    src/mbgl/style/layers/line_layer.cpp \
    src/mbgl/style/layers/line_layer_impl.cpp \
    src/mbgl/style/layers/line_layer_properties.cpp \
    src/mbgl/style/layers/raster_layer.cpp \
    src/mbgl/style/layers/raster_layer_impl.cpp \
    src/mbgl/style/layers/raster_layer_properties.cpp \
    src/mbgl/style/layers/symbol_layer.cpp \
    src/mbgl/style/layers/symbol_layer_impl.cpp \
    src/mbgl/style/layers/symbol_layer_properties.cpp \
    src/mbgl/style/light.cpp \
    src/mbgl/style/light_impl.cpp \
    src/mbgl/style/parser.cpp \
    src/mbgl/style/source.cpp \
    src/mbgl/style/source_impl.cpp \
    src/mbgl/style/sources/custom_geometry_source.cpp \
    src/mbgl/style/sources/custom_geometry_source_impl.cpp \
    src/mbgl/style/sources/geojson_source.cpp \
    src/mbgl/style/sources/geojson_source_impl.cpp \
    src/mbgl/style/sources/image_source.cpp \
    src/mbgl/style/sources/image_source_impl.cpp \
    src/mbgl/style/sources/raster_dem_source.cpp \
    src/mbgl/style/sources/raster_source.cpp \
    src/mbgl/style/sources/raster_source_impl.cpp \
    src/mbgl/style/sources/vector_source.cpp \
    src/mbgl/style/sources/vector_source_impl.cpp \
    src/mbgl/style/style.cpp \
    src/mbgl/style/style_impl.cpp \
    src/mbgl/style/types.cpp \
    src/mbgl/text/check_max_angle.cpp \
    src/mbgl/text/collision_feature.cpp \
    src/mbgl/text/collision_index.cpp \
    src/mbgl/text/cross_tile_symbol_index.cpp \
    src/mbgl/text/get_anchors.cpp \
    src/mbgl/text/glyph.cpp \
    src/mbgl/text/glyph_atlas.cpp \
    src/mbgl/text/glyph_manager.cpp \
    src/mbgl/text/glyph_pbf.cpp \
    src/mbgl/text/language_tag.cpp \
    src/mbgl/text/placement.cpp \
    src/mbgl/text/quads.cpp \
    src/mbgl/text/shaping.cpp \
    src/mbgl/tile/custom_geometry_tile.cpp \
    src/mbgl/tile/geojson_tile.cpp \
    src/mbgl/tile/geometry_tile.cpp \
    src/mbgl/tile/geometry_tile_data.cpp \
    src/mbgl/tile/geometry_tile_worker.cpp \
    src/mbgl/tile/raster_dem_tile.cpp \
    src/mbgl/tile/raster_dem_tile_worker.cpp \
    src/mbgl/tile/raster_tile.cpp \
    src/mbgl/tile/raster_tile_worker.cpp \
    src/mbgl/tile/tile.cpp \
    src/mbgl/tile/tile_cache.cpp \
    src/mbgl/tile/tile_id_hash.cpp \
    src/mbgl/tile/tile_id_io.cpp \
    src/mbgl/tile/vector_tile.cpp \
    src/mbgl/tile/vector_tile_data.cpp \
    src/mbgl/util/chrono.cpp \
    src/mbgl/util/clip_id.cpp \
    src/mbgl/util/color.cpp \
    src/mbgl/util/compression.cpp \
    src/mbgl/util/constants.cpp \
    src/mbgl/util/convert.cpp \
    src/mbgl/util/dtoa.cpp \
    src/mbgl/util/event.cpp \
    src/mbgl/util/font_stack.cpp \
    src/mbgl/util/geo.cpp \
    src/mbgl/util/geojson_impl.cpp \
    src/mbgl/util/grid_index.cpp \
    src/mbgl/util/http_header.cpp \
    src/mbgl/util/http_timeout.cpp \
    src/mbgl/util/i18n.cpp \
    src/mbgl/util/interpolate.cpp \
    src/mbgl/util/intersection_tests.cpp \
    src/mbgl/util/io.cpp \
    src/mbgl/util/logging.cpp \
    src/mbgl/util/mapbox.cpp \
    src/mbgl/util/mat2.cpp \
    src/mbgl/util/mat3.cpp \
    src/mbgl/util/mat4.cpp \
    src/mbgl/util/offscreen_texture.cpp \
    src/mbgl/util/premultiply.cpp \
    src/mbgl/util/stopwatch.cpp \
    src/mbgl/util/string.cpp \
    src/mbgl/util/tile_cover.cpp \
    src/mbgl/util/tile_cover_impl.cpp \
    src/mbgl/util/tiny_sdf.cpp \
    src/mbgl/util/url.cpp \
    src/mbgl/util/version.cpp \
    src/mbgl/util/work_request.cpp \
    src/parsedate/parsedate.cpp \
    platform/default/asset_file_source.cpp \
    platform/default/collator.cpp \
    platform/default/default_file_source.cpp \
    platform/default/file_source_request.cpp \
    platform/default/local_file_source.cpp \
    platform/default/local_glyph_rasterizer.cpp \
    platform/default/mbgl/storage/offline.cpp \
    platform/default/mbgl/storage/offline_database.cpp \
    platform/default/mbgl/storage/offline_download.cpp \
    platform/default/mbgl/util/default_thread_pool.cpp \
    platform/default/mbgl/util/shared_thread_pool.cpp \
    platform/default/online_file_source.cpp \
    platform/default/unaccent.cpp \
    vendor/nunicode/src/libnu/ducet.c \
    vendor/nunicode/src/libnu/strcoll.c \
    vendor/nunicode/src/libnu/strings.c \
    vendor/nunicode/src/libnu/tolower.c \
    vendor/nunicode/src/libnu/tounaccent.c \
    vendor/nunicode/src/libnu/toupper.c \
    vendor/nunicode/src/libnu/utf8.c


HEADERS += \
    platform/qt/include/qmapbox.hpp \
    platform/qt/include/qmapboxgl.hpp \
    platform/qt/src/async_task_impl.hpp \
    platform/qt/src/http_file_source.hpp \
    platform/qt/src/http_request.hpp \
    platform/qt/src/qmapboxgl_map_observer.hpp \
    platform/qt/src/qmapboxgl_map_renderer.hpp \
    platform/qt/src/qmapboxgl_p.hpp \
    platform/qt/src/qmapboxgl_renderer_backend.hpp \
    platform/qt/src/qmapboxgl_renderer_observer.hpp \
    platform/qt/src/qmapboxgl_scheduler.hpp \
    platform/qt/src/qt_conversion.hpp \
    platform/qt/src/qt_geojson.hpp \
    platform/qt/src/run_loop_impl.hpp \
    platform/qt/src/timer_impl.hpp \

INCLUDEPATH += \
    deps/boost/1.65.1 \
    deps/boost/1.65.1/include \
    deps/earcut/0.12.4 \
    deps/earcut/0.12.4/include \
    deps/geojson/0.4.2 \
    deps/geojson/0.4.2/include \
    deps/geojsonvt/6.5.1 \
    deps/geojsonvt/6.5.1/include \
    deps/geometry/0.9.3 \
    deps/geometry/0.9.3/include \
    deps/kdbush/0.1.1-1 \
    deps/kdbush/0.1.1-1/include \
    deps/optional/f27e7908 \
    deps/optional/f27e7908/include \
    deps/polylabel/1.0.3 \
    deps/polylabel/1.0.3/include \
    deps/protozero/1.5.2 \
    deps/protozero/1.5.2/include \
    deps/rapidjson/1.1.0 \
    deps/rapidjson/1.1.0/include \
    deps/shelf-pack/2.1.1 \
    deps/shelf-pack/2.1.1/include \
    deps/supercluster/0.2.2 \
    deps/supercluster/0.2.2/include \
    deps/tao_tuple/28626e99 \
    deps/tao_tuple/28626e99/include \
    deps/unique_resource/cba309e \
    deps/unique_resource/cba309e/include \
    deps/variant/1.1.4 \
    deps/variant/1.1.4/include \
    deps/vector-tile/1.0.2 \
    deps/vector-tile/1.0.2/include \
    deps/wagyu/0.4.3 \
    deps/wagyu/0.4.3/include \
    include \
    platform/default \
    platform/qt \
    platform/qt/include \
    src \
    vendor/nunicode/include

QMAKE_CXXFLAGS += \
    -DMBGL_VERSION_REV=\\\"qt-v1.5.0\\\"
