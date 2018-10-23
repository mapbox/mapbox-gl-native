cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(MAPBOX_GL_NATIVE LANGUAGES CXX C)

set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo MinSizeRel)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Required to avoid warnings
# when setting visibility.
cmake_policy(SET CMP0063 NEW)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(GIT_ROOT ${CMAKE_SOURCE_DIR}/..)

add_library(mapbox-gl-native STATIC
    ${GIT_ROOT}/src/csscolorparser/csscolorparser.cpp
    ${GIT_ROOT}/src/mbgl/actor/mailbox.cpp
    ${GIT_ROOT}/src/mbgl/actor/scheduler.cpp
    ${GIT_ROOT}/src/mbgl/algorithm/generate_clip_ids.cpp
    ${GIT_ROOT}/src/mbgl/annotation/annotation_manager.cpp
    ${GIT_ROOT}/src/mbgl/annotation/annotation_source.cpp
    ${GIT_ROOT}/src/mbgl/annotation/annotation_tile.cpp
    ${GIT_ROOT}/src/mbgl/annotation/fill_annotation_impl.cpp
    ${GIT_ROOT}/src/mbgl/annotation/line_annotation_impl.cpp
    ${GIT_ROOT}/src/mbgl/annotation/render_annotation_source.cpp
    ${GIT_ROOT}/src/mbgl/annotation/shape_annotation_impl.cpp
    ${GIT_ROOT}/src/mbgl/annotation/symbol_annotation_impl.cpp
    ${GIT_ROOT}/src/mbgl/geometry/dem_data.cpp
    ${GIT_ROOT}/src/mbgl/geometry/feature_index.cpp
    ${GIT_ROOT}/src/mbgl/geometry/line_atlas.cpp
    ${GIT_ROOT}/src/mbgl/gl/attribute.cpp
    ${GIT_ROOT}/src/mbgl/gl/color_mode.cpp
    ${GIT_ROOT}/src/mbgl/gl/context.cpp
    ${GIT_ROOT}/src/mbgl/gl/cull_face_mode.cpp
    ${GIT_ROOT}/src/mbgl/gl/debugging.cpp
    ${GIT_ROOT}/src/mbgl/gl/debugging_extension.cpp
    ${GIT_ROOT}/src/mbgl/gl/depth_mode.cpp
    ${GIT_ROOT}/src/mbgl/gl/gl.cpp
    ${GIT_ROOT}/src/mbgl/gl/object.cpp
    ${GIT_ROOT}/src/mbgl/gl/stencil_mode.cpp
    ${GIT_ROOT}/src/mbgl/gl/uniform.cpp
    ${GIT_ROOT}/src/mbgl/gl/value.cpp
    ${GIT_ROOT}/src/mbgl/gl/vertex_array.cpp
    ${GIT_ROOT}/src/mbgl/layout/clip_lines.cpp
    ${GIT_ROOT}/src/mbgl/layout/merge_lines.cpp
    ${GIT_ROOT}/src/mbgl/layout/symbol_instance.cpp
    ${GIT_ROOT}/src/mbgl/layout/symbol_layout.cpp
    ${GIT_ROOT}/src/mbgl/layout/symbol_projection.cpp
    ${GIT_ROOT}/src/mbgl/map/map.cpp
    ${GIT_ROOT}/src/mbgl/map/transform.cpp
    ${GIT_ROOT}/src/mbgl/map/transform_state.cpp
    ${GIT_ROOT}/src/mbgl/math/log2.cpp
    ${GIT_ROOT}/src/mbgl/programs/background_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/binary_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/circle_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/collision_box_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/extrusion_texture_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/fill_extrusion_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/fill_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/heatmap_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/heatmap_texture_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/hillshade_prepare_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/hillshade_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/line_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/program_parameters.cpp
    ${GIT_ROOT}/src/mbgl/programs/raster_program.cpp
    ${GIT_ROOT}/src/mbgl/programs/symbol_program.cpp
    ${GIT_ROOT}/src/mbgl/renderer/backend_scope.cpp
    ${GIT_ROOT}/src/mbgl/renderer/bucket_parameters.cpp
    ${GIT_ROOT}/src/mbgl/renderer/buckets/circle_bucket.cpp
    ${GIT_ROOT}/src/mbgl/renderer/buckets/debug_bucket.cpp
    ${GIT_ROOT}/src/mbgl/renderer/buckets/fill_bucket.cpp
    ${GIT_ROOT}/src/mbgl/renderer/buckets/fill_extrusion_bucket.cpp
    ${GIT_ROOT}/src/mbgl/renderer/buckets/heatmap_bucket.cpp
    ${GIT_ROOT}/src/mbgl/renderer/buckets/hillshade_bucket.cpp
    ${GIT_ROOT}/src/mbgl/renderer/buckets/line_bucket.cpp
    ${GIT_ROOT}/src/mbgl/renderer/buckets/raster_bucket.cpp
    ${GIT_ROOT}/src/mbgl/renderer/buckets/symbol_bucket.cpp
    ${GIT_ROOT}/src/mbgl/renderer/cross_faded_property_evaluator.cpp
    ${GIT_ROOT}/src/mbgl/renderer/group_by_layout.cpp
    ${GIT_ROOT}/src/mbgl/renderer/image_atlas.cpp
    ${GIT_ROOT}/src/mbgl/renderer/image_manager.cpp
    ${GIT_ROOT}/src/mbgl/renderer/layers/render_background_layer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/layers/render_circle_layer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/layers/render_custom_layer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/layers/render_fill_extrusion_layer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/layers/render_fill_layer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/layers/render_heatmap_layer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/layers/render_hillshade_layer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/layers/render_line_layer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/layers/render_raster_layer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/layers/render_symbol_layer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/paint_parameters.cpp
    ${GIT_ROOT}/src/mbgl/renderer/render_layer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/render_light.cpp
    ${GIT_ROOT}/src/mbgl/renderer/render_source.cpp
    ${GIT_ROOT}/src/mbgl/renderer/render_static_data.cpp
    ${GIT_ROOT}/src/mbgl/renderer/render_tile.cpp
    ${GIT_ROOT}/src/mbgl/renderer/renderer.cpp
    ${GIT_ROOT}/src/mbgl/renderer/renderer_backend.cpp
    ${GIT_ROOT}/src/mbgl/renderer/renderer_impl.cpp
    ${GIT_ROOT}/src/mbgl/renderer/renderer_state.cpp
    ${GIT_ROOT}/src/mbgl/renderer/sources/render_custom_geometry_source.cpp
    ${GIT_ROOT}/src/mbgl/renderer/sources/render_geojson_source.cpp
    ${GIT_ROOT}/src/mbgl/renderer/sources/render_image_source.cpp
    ${GIT_ROOT}/src/mbgl/renderer/sources/render_raster_dem_source.cpp
    ${GIT_ROOT}/src/mbgl/renderer/sources/render_raster_source.cpp
    ${GIT_ROOT}/src/mbgl/renderer/sources/render_vector_source.cpp
    ${GIT_ROOT}/src/mbgl/renderer/style_diff.cpp
    ${GIT_ROOT}/src/mbgl/renderer/tile_pyramid.cpp
    ${GIT_ROOT}/src/mbgl/shaders/background.cpp
    ${GIT_ROOT}/src/mbgl/shaders/background_pattern.cpp
    ${GIT_ROOT}/src/mbgl/shaders/circle.cpp
    ${GIT_ROOT}/src/mbgl/shaders/clipping_mask.cpp
    ${GIT_ROOT}/src/mbgl/shaders/collision_box.cpp
    ${GIT_ROOT}/src/mbgl/shaders/collision_circle.cpp
    ${GIT_ROOT}/src/mbgl/shaders/debug.cpp
    ${GIT_ROOT}/src/mbgl/shaders/extrusion_texture.cpp
    ${GIT_ROOT}/src/mbgl/shaders/fill.cpp
    ${GIT_ROOT}/src/mbgl/shaders/fill_extrusion.cpp
    ${GIT_ROOT}/src/mbgl/shaders/fill_extrusion_pattern.cpp
    ${GIT_ROOT}/src/mbgl/shaders/fill_outline.cpp
    ${GIT_ROOT}/src/mbgl/shaders/fill_outline_pattern.cpp
    ${GIT_ROOT}/src/mbgl/shaders/fill_pattern.cpp
    ${GIT_ROOT}/src/mbgl/shaders/heatmap.cpp
    ${GIT_ROOT}/src/mbgl/shaders/heatmap_texture.cpp
    ${GIT_ROOT}/src/mbgl/shaders/hillshade.cpp
    ${GIT_ROOT}/src/mbgl/shaders/hillshade_prepare.cpp
    ${GIT_ROOT}/src/mbgl/shaders/line.cpp
    ${GIT_ROOT}/src/mbgl/shaders/line_gradient.cpp
    ${GIT_ROOT}/src/mbgl/shaders/line_pattern.cpp
    ${GIT_ROOT}/src/mbgl/shaders/line_sdf.cpp
    ${GIT_ROOT}/src/mbgl/shaders/preludes.cpp
    ${GIT_ROOT}/src/mbgl/shaders/raster.cpp
    ${GIT_ROOT}/src/mbgl/shaders/shaders.cpp
    ${GIT_ROOT}/src/mbgl/shaders/source.cpp
    ${GIT_ROOT}/src/mbgl/shaders/symbol_icon.cpp
    ${GIT_ROOT}/src/mbgl/shaders/symbol_sdf.cpp
    ${GIT_ROOT}/src/mbgl/sprite/sprite_loader.cpp
    ${GIT_ROOT}/src/mbgl/sprite/sprite_loader_worker.cpp
    ${GIT_ROOT}/src/mbgl/sprite/sprite_parser.cpp
    ${GIT_ROOT}/src/mbgl/storage/network_status.cpp
    ${GIT_ROOT}/src/mbgl/storage/resource.cpp
    ${GIT_ROOT}/src/mbgl/storage/resource_transform.cpp
    ${GIT_ROOT}/src/mbgl/storage/response.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/color_ramp_property_value.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/constant.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/coordinate.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/custom_geometry_source_options.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/filter.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/function.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/geojson.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/geojson_options.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/get_json_type.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/layer.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/light.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/position.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/property_value.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/source.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/tileset.cpp
    ${GIT_ROOT}/src/mbgl/style/conversion/transition_options.cpp
    ${GIT_ROOT}/src/mbgl/style/custom_tile_loader.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/assertion.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/at.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/boolean_operator.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/case.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/check_subtype.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/coalesce.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/coercion.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/collator_expression.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/comparison.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/compound_expression.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/dsl.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/expression.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/find_zoom_curve.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/format_expression.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/formatted.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/get_covering_stops.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/interpolate.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/is_constant.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/is_expression.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/length.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/let.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/literal.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/match.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/parsing_context.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/step.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/util.cpp
    ${GIT_ROOT}/src/mbgl/style/expression/value.cpp
    ${GIT_ROOT}/src/mbgl/style/filter.cpp
    ${GIT_ROOT}/src/mbgl/style/image.cpp
    ${GIT_ROOT}/src/mbgl/style/image_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layer.cpp
    ${GIT_ROOT}/src/mbgl/style/layer_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/background_layer.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/background_layer_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/background_layer_properties.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/circle_layer.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/circle_layer_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/circle_layer_properties.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/custom_layer.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/custom_layer_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/fill_extrusion_layer.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/fill_extrusion_layer_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/fill_extrusion_layer_properties.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/fill_layer.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/fill_layer_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/fill_layer_properties.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/heatmap_layer.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/heatmap_layer_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/heatmap_layer_properties.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/hillshade_layer.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/hillshade_layer_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/hillshade_layer_properties.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/line_layer.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/line_layer_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/line_layer_properties.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/raster_layer.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/raster_layer_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/raster_layer_properties.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/symbol_layer.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/symbol_layer_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/layers/symbol_layer_properties.cpp
    ${GIT_ROOT}/src/mbgl/style/light.cpp
    ${GIT_ROOT}/src/mbgl/style/light_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/parser.cpp
    ${GIT_ROOT}/src/mbgl/style/source.cpp
    ${GIT_ROOT}/src/mbgl/style/source_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/sources/custom_geometry_source.cpp
    ${GIT_ROOT}/src/mbgl/style/sources/custom_geometry_source_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/sources/geojson_source.cpp
    ${GIT_ROOT}/src/mbgl/style/sources/geojson_source_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/sources/image_source.cpp
    ${GIT_ROOT}/src/mbgl/style/sources/image_source_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/sources/raster_dem_source.cpp
    ${GIT_ROOT}/src/mbgl/style/sources/raster_source.cpp
    ${GIT_ROOT}/src/mbgl/style/sources/raster_source_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/sources/vector_source.cpp
    ${GIT_ROOT}/src/mbgl/style/sources/vector_source_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/style.cpp
    ${GIT_ROOT}/src/mbgl/style/style_impl.cpp
    ${GIT_ROOT}/src/mbgl/style/types.cpp
    ${GIT_ROOT}/src/mbgl/text/check_max_angle.cpp
    ${GIT_ROOT}/src/mbgl/text/collision_feature.cpp
    ${GIT_ROOT}/src/mbgl/text/collision_index.cpp
    ${GIT_ROOT}/src/mbgl/text/cross_tile_symbol_index.cpp
    ${GIT_ROOT}/src/mbgl/text/get_anchors.cpp
    ${GIT_ROOT}/src/mbgl/text/glyph.cpp
    ${GIT_ROOT}/src/mbgl/text/glyph_atlas.cpp
    ${GIT_ROOT}/src/mbgl/text/glyph_manager.cpp
    ${GIT_ROOT}/src/mbgl/text/glyph_pbf.cpp
    ${GIT_ROOT}/src/mbgl/text/language_tag.cpp
    ${GIT_ROOT}/src/mbgl/text/placement.cpp
    ${GIT_ROOT}/src/mbgl/text/quads.cpp
    ${GIT_ROOT}/src/mbgl/text/shaping.cpp
    ${GIT_ROOT}/src/mbgl/text/tagged_string.cpp
    ${GIT_ROOT}/src/mbgl/tile/custom_geometry_tile.cpp
    ${GIT_ROOT}/src/mbgl/tile/geojson_tile.cpp
    ${GIT_ROOT}/src/mbgl/tile/geometry_tile.cpp
    ${GIT_ROOT}/src/mbgl/tile/geometry_tile_data.cpp
    ${GIT_ROOT}/src/mbgl/tile/geometry_tile_worker.cpp
    ${GIT_ROOT}/src/mbgl/tile/raster_dem_tile.cpp
    ${GIT_ROOT}/src/mbgl/tile/raster_dem_tile_worker.cpp
    ${GIT_ROOT}/src/mbgl/tile/raster_tile.cpp
    ${GIT_ROOT}/src/mbgl/tile/raster_tile_worker.cpp
    ${GIT_ROOT}/src/mbgl/tile/tile.cpp
    ${GIT_ROOT}/src/mbgl/tile/tile_cache.cpp
    ${GIT_ROOT}/src/mbgl/tile/tile_id_hash.cpp
    ${GIT_ROOT}/src/mbgl/tile/tile_id_io.cpp
    ${GIT_ROOT}/src/mbgl/tile/vector_tile.cpp
    ${GIT_ROOT}/src/mbgl/tile/vector_tile_data.cpp
    ${GIT_ROOT}/src/mbgl/util/chrono.cpp
    ${GIT_ROOT}/src/mbgl/util/clip_id.cpp
    ${GIT_ROOT}/src/mbgl/util/color.cpp
    ${GIT_ROOT}/src/mbgl/util/compression.cpp
    ${GIT_ROOT}/src/mbgl/util/constants.cpp
    ${GIT_ROOT}/src/mbgl/util/convert.cpp
    ${GIT_ROOT}/src/mbgl/util/dtoa.cpp
    ${GIT_ROOT}/src/mbgl/util/event.cpp
    ${GIT_ROOT}/src/mbgl/util/font_stack.cpp
    ${GIT_ROOT}/src/mbgl/util/geo.cpp
    ${GIT_ROOT}/src/mbgl/util/geojson_impl.cpp
    ${GIT_ROOT}/src/mbgl/util/grid_index.cpp
    ${GIT_ROOT}/src/mbgl/util/http_header.cpp
    ${GIT_ROOT}/src/mbgl/util/http_timeout.cpp
    ${GIT_ROOT}/src/mbgl/util/i18n.cpp
    ${GIT_ROOT}/src/mbgl/util/interpolate.cpp
    ${GIT_ROOT}/src/mbgl/util/intersection_tests.cpp
    ${GIT_ROOT}/src/mbgl/util/io.cpp
    ${GIT_ROOT}/src/mbgl/util/logging.cpp
    ${GIT_ROOT}/src/mbgl/util/mapbox.cpp
    ${GIT_ROOT}/src/mbgl/util/mat2.cpp
    ${GIT_ROOT}/src/mbgl/util/mat3.cpp
    ${GIT_ROOT}/src/mbgl/util/mat4.cpp
    ${GIT_ROOT}/src/mbgl/util/offscreen_texture.cpp
    ${GIT_ROOT}/src/mbgl/util/premultiply.cpp
    ${GIT_ROOT}/src/mbgl/util/stopwatch.cpp
    ${GIT_ROOT}/src/mbgl/util/string.cpp
    ${GIT_ROOT}/src/mbgl/util/tile_cover.cpp
    ${GIT_ROOT}/src/mbgl/util/tile_cover_impl.cpp
    ${GIT_ROOT}/src/mbgl/util/tiny_sdf.cpp
    ${GIT_ROOT}/src/mbgl/util/url.cpp
    ${GIT_ROOT}/src/mbgl/util/version.cpp
    ${GIT_ROOT}/src/mbgl/util/work_request.cpp
    ${GIT_ROOT}/src/parsedate/parsedate.cpp
)

target_compile_definitions(mapbox-gl-native PRIVATE
    MBGL_VERSION_REV="1.0"
)

# FIXME: Vendor the parts of boost we need.
find_package(Boost 1.65 REQUIRED)

target_include_directories(mapbox-gl-native PRIVATE
    ${GIT_ROOT}/include
    ${GIT_ROOT}/src
    ${Boost_INCLUDE_DIRS}
)

include(${GIT_ROOT}/vendor/earcut.hpp.cmake)
include(${GIT_ROOT}/vendor/expected.cmake)
include(${GIT_ROOT}/vendor/geojson-vt-cpp.cmake)
include(${GIT_ROOT}/vendor/geojson.hpp.cmake)
include(${GIT_ROOT}/vendor/geometry.hpp.cmake)
include(${GIT_ROOT}/vendor/icu.cmake)
include(${GIT_ROOT}/vendor/kdbush.hpp.cmake)
include(${GIT_ROOT}/vendor/nunicode.cmake)
include(${GIT_ROOT}/vendor/polylabel.cmake)
include(${GIT_ROOT}/vendor/protozero.cmake)
include(${GIT_ROOT}/vendor/rapidjson.cmake)
include(${GIT_ROOT}/vendor/shelf-pack-cpp.cmake)
include(${GIT_ROOT}/vendor/supercluster.hpp.cmake)
include(${GIT_ROOT}/vendor/unique_resource.cmake)
include(${GIT_ROOT}/vendor/variant.cmake)
include(${GIT_ROOT}/vendor/vector-tile.cmake)
include(${GIT_ROOT}/vendor/wagyu.cmake)

target_link_libraries(mapbox-gl-native PRIVATE
    earcut.hpp
    geojson-vt-cpp
    kdbush.hpp
    polylabel
    protozero
    rapidjson
    shelf-pack-cpp
    supercluster.hpp
    unique_resource
    vector-tile
    wagyu
)

# FIXME: These need to be public because they
# appear in public headers. We should not leak
# any external dependency.
target_link_libraries(mapbox-gl-native PUBLIC
    expected
    geojson.hpp
    geometry.hpp
    icu
    nunicode
    variant
)

# FIXME: This is need for the GL headers, but
# the build should be self contained.
target_include_directories(mapbox-gl-native PRIVATE
    ${GIT_ROOT}/platform/default
    ${GIT_ROOT}/platform/linux
)

if(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(mapbox-gl-native PUBLIC
        ${CMAKE_THREAD_LIBS_INIT}
    )
endif()

if(WIN32)
    include(${GIT_ROOT}/deps/optional.cmake)
    target_link_libraries(mapbox-gl-native PUBLIC optional)

    target_compile_definitions(mapbox-gl-native PRIVATE
        _WINDOWS
    )
endif()

target_include_directories(mapbox-gl-native SYSTEM INTERFACE
    ${GIT_ROOT}/include
)
