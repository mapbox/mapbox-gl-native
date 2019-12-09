add_library(
    mbgl-test SHARED EXCLUDE_FROM_ALL
    ${MBGL_ROOT}/test/actor/actor.test.cpp
    ${MBGL_ROOT}/test/actor/actor_ref.test.cpp
    ${MBGL_ROOT}/test/algorithm/update_renderables.test.cpp
    ${MBGL_ROOT}/test/algorithm/update_tile_masks.test.cpp
    ${MBGL_ROOT}/test/api/annotations.test.cpp
    ${MBGL_ROOT}/test/api/api_misuse.test.cpp
    ${MBGL_ROOT}/test/api/custom_geometry_source.test.cpp
    ${MBGL_ROOT}/test/api/custom_layer.test.cpp
    ${MBGL_ROOT}/test/api/query.test.cpp
    ${MBGL_ROOT}/test/api/recycle_map.cpp
    ${MBGL_ROOT}/test/geometry/dem_data.test.cpp
    ${MBGL_ROOT}/test/geometry/line_atlas.test.cpp
    ${MBGL_ROOT}/test/gl/bucket.test.cpp
    ${MBGL_ROOT}/test/gl/context.test.cpp
    ${MBGL_ROOT}/test/gl/gl_functions.test.cpp
    ${MBGL_ROOT}/test/gl/object.test.cpp
    ${MBGL_ROOT}/test/map/map.test.cpp
    ${MBGL_ROOT}/test/map/prefetch.test.cpp
    ${MBGL_ROOT}/test/map/transform.test.cpp
    ${MBGL_ROOT}/test/math/clamp.test.cpp
    ${MBGL_ROOT}/test/math/minmax.test.cpp
    ${MBGL_ROOT}/test/math/wrap.test.cpp
    ${MBGL_ROOT}/test/programs/symbol_program.test.cpp
    ${MBGL_ROOT}/test/renderer/backend_scope.test.cpp
    ${MBGL_ROOT}/test/renderer/image_manager.test.cpp
    ${MBGL_ROOT}/test/renderer/pattern_atlas.test.cpp
    ${MBGL_ROOT}/test/sprite/sprite_loader.test.cpp
    ${MBGL_ROOT}/test/sprite/sprite_parser.test.cpp
    ${MBGL_ROOT}/test/src/mbgl/test/fixture_log_observer.cpp
    ${MBGL_ROOT}/test/src/mbgl/test/getrss.cpp
    ${MBGL_ROOT}/test/src/mbgl/test/sqlite3_test_fs.cpp
    ${MBGL_ROOT}/test/src/mbgl/test/stub_file_source.cpp
    ${MBGL_ROOT}/test/src/mbgl/test/test.cpp
    ${MBGL_ROOT}/test/src/mbgl/test/util.cpp
    ${MBGL_ROOT}/test/storage/asset_file_source.test.cpp
    ${MBGL_ROOT}/test/storage/default_file_source.test.cpp
    ${MBGL_ROOT}/test/storage/headers.test.cpp
    ${MBGL_ROOT}/test/storage/http_file_source.test.cpp
    ${MBGL_ROOT}/test/storage/local_file_source.test.cpp
    ${MBGL_ROOT}/test/storage/offline.test.cpp
    ${MBGL_ROOT}/test/storage/offline_database.test.cpp
    ${MBGL_ROOT}/test/storage/offline_download.test.cpp
    ${MBGL_ROOT}/test/storage/online_file_source.test.cpp
    ${MBGL_ROOT}/test/storage/resource.test.cpp
    ${MBGL_ROOT}/test/storage/sqlite.test.cpp
    ${MBGL_ROOT}/test/style/conversion/conversion_impl.test.cpp
    ${MBGL_ROOT}/test/style/conversion/function.test.cpp
    ${MBGL_ROOT}/test/style/conversion/geojson_options.test.cpp
    ${MBGL_ROOT}/test/style/conversion/layer.test.cpp
    ${MBGL_ROOT}/test/style/conversion/light.test.cpp
    ${MBGL_ROOT}/test/style/conversion/property_value.test.cpp
    ${MBGL_ROOT}/test/style/conversion/stringify.test.cpp
    ${MBGL_ROOT}/test/style/conversion/tileset.test.cpp
    ${MBGL_ROOT}/test/style/expression/expression.test.cpp
    ${MBGL_ROOT}/test/style/expression/util.test.cpp
    ${MBGL_ROOT}/test/style/filter.test.cpp
    ${MBGL_ROOT}/test/style/properties.test.cpp
    ${MBGL_ROOT}/test/style/property_expression.test.cpp
    ${MBGL_ROOT}/test/style/source.test.cpp
    ${MBGL_ROOT}/test/style/style.test.cpp
    ${MBGL_ROOT}/test/style/style_image.test.cpp
    ${MBGL_ROOT}/test/style/style_layer.test.cpp
    ${MBGL_ROOT}/test/style/style_parser.test.cpp
    ${MBGL_ROOT}/test/text/bidi.test.cpp
    ${MBGL_ROOT}/test/text/calculate_tile_distances.test.cpp
    ${MBGL_ROOT}/test/text/cross_tile_symbol_index.test.cpp
    ${MBGL_ROOT}/test/text/formatted.test.cpp
    ${MBGL_ROOT}/test/text/get_anchors.test.cpp
    ${MBGL_ROOT}/test/text/glyph_manager.test.cpp
    ${MBGL_ROOT}/test/text/glyph_pbf.test.cpp
    ${MBGL_ROOT}/test/text/language_tag.test.cpp
    ${MBGL_ROOT}/test/text/local_glyph_rasterizer.test.cpp
    ${MBGL_ROOT}/test/text/quads.test.cpp
    ${MBGL_ROOT}/test/text/shaping.test.cpp
    ${MBGL_ROOT}/test/text/tagged_string.test.cpp
    ${MBGL_ROOT}/test/tile/custom_geometry_tile.test.cpp
    ${MBGL_ROOT}/test/tile/geojson_tile.test.cpp
    ${MBGL_ROOT}/test/tile/geometry_tile_data.test.cpp
    ${MBGL_ROOT}/test/tile/raster_dem_tile.test.cpp
    ${MBGL_ROOT}/test/tile/raster_tile.test.cpp
    ${MBGL_ROOT}/test/tile/tile_cache.test.cpp
    ${MBGL_ROOT}/test/tile/tile_coordinate.test.cpp
    ${MBGL_ROOT}/test/tile/tile_id.test.cpp
    ${MBGL_ROOT}/test/tile/vector_tile.test.cpp
    ${MBGL_ROOT}/test/util/async_task.test.cpp
    ${MBGL_ROOT}/test/util/dtoa.test.cpp
    ${MBGL_ROOT}/test/util/geo.test.cpp
    ${MBGL_ROOT}/test/util/grid_index.test.cpp
    ${MBGL_ROOT}/test/util/http_timeout.test.cpp
    ${MBGL_ROOT}/test/util/image.test.cpp
    ${MBGL_ROOT}/test/util/mapbox.test.cpp
    ${MBGL_ROOT}/test/util/memory.test.cpp
    ${MBGL_ROOT}/test/util/merge_lines.test.cpp
    ${MBGL_ROOT}/test/util/number_conversions.test.cpp
    ${MBGL_ROOT}/test/util/offscreen_texture.test.cpp
    ${MBGL_ROOT}/test/util/position.test.cpp
    ${MBGL_ROOT}/test/util/projection.test.cpp
    ${MBGL_ROOT}/test/util/run_loop.test.cpp
    ${MBGL_ROOT}/test/util/string.test.cpp
    ${MBGL_ROOT}/test/util/text_conversions.test.cpp
    ${MBGL_ROOT}/test/util/thread.test.cpp
    ${MBGL_ROOT}/test/util/thread_local.test.cpp
    ${MBGL_ROOT}/test/util/tile_cover.test.cpp
    ${MBGL_ROOT}/test/util/tile_range.test.cpp
    ${MBGL_ROOT}/test/util/timer.test.cpp
    ${MBGL_ROOT}/test/util/token.test.cpp
    ${MBGL_ROOT}/test/util/url.test.cpp
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
        ${MBGL_ROOT}/platform/default/include
        ${MBGL_ROOT}/platform/gfx/gl/src
        ${MBGL_ROOT}/src
        ${MBGL_ROOT}/test/src
)

target_include_directories(
    mbgl-test
    PUBLIC ${MBGL_ROOT}/include ${MBGL_ROOT}/test/include
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
        mbgl-core
        $<$<PLATFORM_ID:Android>:mbgl-core-android>
)

target_link_libraries(
    mbgl-test
    PUBLIC mbgl-vendor-googletest
)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    set_target_properties(mbgl-test PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=safe")
endif()

set_property(TARGET mbgl-test PROPERTY FOLDER Core)
