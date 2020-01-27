add_library(
    mbgl-test STATIC EXCLUDE_FROM_ALL
    ${PROJECT_SOURCE_DIR}/test/actor/actor.test.cpp
    ${PROJECT_SOURCE_DIR}/test/actor/actor_ref.test.cpp
    ${PROJECT_SOURCE_DIR}/test/algorithm/update_renderables.test.cpp
    ${PROJECT_SOURCE_DIR}/test/algorithm/update_tile_masks.test.cpp
    ${PROJECT_SOURCE_DIR}/test/api/annotations.test.cpp
    ${PROJECT_SOURCE_DIR}/test/api/api_misuse.test.cpp
    ${PROJECT_SOURCE_DIR}/test/api/custom_geometry_source.test.cpp
    ${PROJECT_SOURCE_DIR}/test/api/custom_layer.test.cpp
    ${PROJECT_SOURCE_DIR}/test/api/query.test.cpp
    ${PROJECT_SOURCE_DIR}/test/api/recycle_map.cpp
    ${PROJECT_SOURCE_DIR}/test/geometry/dem_data.test.cpp
    ${PROJECT_SOURCE_DIR}/test/geometry/line_atlas.test.cpp
    ${PROJECT_SOURCE_DIR}/test/gl/bucket.test.cpp
    ${PROJECT_SOURCE_DIR}/test/gl/context.test.cpp
    ${PROJECT_SOURCE_DIR}/test/gl/gl_functions.test.cpp
    ${PROJECT_SOURCE_DIR}/test/gl/object.test.cpp
    ${PROJECT_SOURCE_DIR}/test/map/map.test.cpp
    ${PROJECT_SOURCE_DIR}/test/map/prefetch.test.cpp
    ${PROJECT_SOURCE_DIR}/test/map/transform.test.cpp
    ${PROJECT_SOURCE_DIR}/test/math/clamp.test.cpp
    ${PROJECT_SOURCE_DIR}/test/math/minmax.test.cpp
    ${PROJECT_SOURCE_DIR}/test/math/wrap.test.cpp
    ${PROJECT_SOURCE_DIR}/test/programs/symbol_program.test.cpp
    ${PROJECT_SOURCE_DIR}/test/renderer/backend_scope.test.cpp
    ${PROJECT_SOURCE_DIR}/test/renderer/image_manager.test.cpp
    ${PROJECT_SOURCE_DIR}/test/renderer/pattern_atlas.test.cpp
    ${PROJECT_SOURCE_DIR}/test/sprite/sprite_loader.test.cpp
    ${PROJECT_SOURCE_DIR}/test/sprite/sprite_parser.test.cpp
    ${PROJECT_SOURCE_DIR}/test/src/mbgl/test/fixture_log_observer.cpp
    ${PROJECT_SOURCE_DIR}/test/src/mbgl/test/getrss.cpp
    ${PROJECT_SOURCE_DIR}/test/src/mbgl/test/sqlite3_test_fs.cpp
    ${PROJECT_SOURCE_DIR}/test/src/mbgl/test/stub_file_source.cpp
    ${PROJECT_SOURCE_DIR}/test/src/mbgl/test/test.cpp
    ${PROJECT_SOURCE_DIR}/test/src/mbgl/test/util.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/asset_file_source.test.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/database_file_source.test.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/headers.test.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/http_file_source.test.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/local_file_source.test.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/main_resource_loader.test.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/offline.test.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/offline_database.test.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/offline_download.test.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/online_file_source.test.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/resource.test.cpp
    ${PROJECT_SOURCE_DIR}/test/storage/sqlite.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/conversion/conversion_impl.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/conversion/function.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/conversion/geojson_options.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/conversion/layer.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/conversion/light.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/conversion/property_value.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/conversion/stringify.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/conversion/tileset.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/expression/expression.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/expression/util.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/filter.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/properties.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/property_expression.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/source.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/style.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/style_image.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/style_layer.test.cpp
    ${PROJECT_SOURCE_DIR}/test/style/style_parser.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/bidi.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/calculate_tile_distances.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/cross_tile_symbol_index.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/formatted.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/get_anchors.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/glyph_manager.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/glyph_pbf.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/language_tag.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/local_glyph_rasterizer.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/quads.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/shaping.test.cpp
    ${PROJECT_SOURCE_DIR}/test/text/tagged_string.test.cpp
    ${PROJECT_SOURCE_DIR}/test/tile/custom_geometry_tile.test.cpp
    ${PROJECT_SOURCE_DIR}/test/tile/geojson_tile.test.cpp
    ${PROJECT_SOURCE_DIR}/test/tile/geometry_tile_data.test.cpp
    ${PROJECT_SOURCE_DIR}/test/tile/raster_dem_tile.test.cpp
    ${PROJECT_SOURCE_DIR}/test/tile/raster_tile.test.cpp
    ${PROJECT_SOURCE_DIR}/test/tile/tile_cache.test.cpp
    ${PROJECT_SOURCE_DIR}/test/tile/tile_coordinate.test.cpp
    ${PROJECT_SOURCE_DIR}/test/tile/tile_id.test.cpp
    ${PROJECT_SOURCE_DIR}/test/tile/vector_tile.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/async_task.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/dtoa.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/geo.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/grid_index.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/http_timeout.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/image.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/mapbox.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/memory.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/merge_lines.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/number_conversions.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/offscreen_texture.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/position.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/projection.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/run_loop.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/string.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/text_conversions.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/thread.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/thread_local.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/tile_cover.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/tile_range.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/timer.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/token.test.cpp
    ${PROJECT_SOURCE_DIR}/test/util/url.test.cpp
)

find_program(MBGL_TEST_NODEJS NAMES nodejs node)
find_program(MBGL_TEST_NPM NAMES npm)

if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL Android OR NOT MBGL_TEST_NODEJS OR NOT MBGL_TEST_NPM)
    message("Target platform does not support HTTP tests or dependencies not found.")

    set(MBGL_TEST_HAS_TEST_SERVER 0)
else()
    set(MBGL_TEST_HAS_TEST_SERVER 1)
endif()

if(NOT DEFINED ENV{CI})
    set(MBGL_TEST_BUILD_ON_CI 0)
else()
    set(MBGL_TEST_BUILD_ON_CI 1)
endif()

target_compile_definitions(
    mbgl-test
    PRIVATE NODE_EXECUTABLE=${MBGL_TEST_NODEJS} TEST_HAS_SERVER=${MBGL_TEST_HAS_TEST_SERVER} CI_BUILD=${MBGL_TEST_BUILD_ON_CI}
)

target_include_directories(
    mbgl-test
    PRIVATE
        ${PROJECT_SOURCE_DIR}/platform/default/include
        ${PROJECT_SOURCE_DIR}/platform/gfx/gl/src
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/test/src
)

target_include_directories(
    mbgl-test
    PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/test/include
)

include(${PROJECT_SOURCE_DIR}/vendor/googletest.cmake)

# Needed for testing private classes
get_target_property(MBGL_CORE_PRIVATE_LIBRARIES mbgl-core LINK_LIBRARIES)

target_link_libraries(
    mbgl-test
    PRIVATE
        ${MBGL_CORE_PRIVATE_LIBRARIES}
        Mapbox::Base::Extras::args
        Mapbox::Base::pixelmatch-cpp
        mbgl-compiler-options
    PUBLIC mbgl-core
)

target_link_libraries(
    mbgl-test
    PUBLIC mbgl-vendor-googletest
)

set_property(TARGET mbgl-test PROPERTY FOLDER Core)
