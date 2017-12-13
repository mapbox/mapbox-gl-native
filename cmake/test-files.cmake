# Do not edit. Regenerate this with ./scripts/generate-test-files.sh

set(MBGL_TEST_FILES
    # actor
    test/actor/actor.test.cpp
    test/actor/actor_ref.test.cpp

    # algorithm
    test/algorithm/covered_by_children.test.cpp
    test/algorithm/generate_clip_ids.test.cpp
    test/algorithm/mock.hpp
    test/algorithm/update_renderables.test.cpp
    test/algorithm/update_tile_masks.test.cpp

    # api
    test/api/annotations.test.cpp
    test/api/api_misuse.test.cpp
    test/api/custom_geometry_source.test.cpp
    test/api/custom_layer.test.cpp
    test/api/query.test.cpp
    test/api/recycle_map.cpp
    test/api/zoom_history.cpp

    # gl
    test/gl/bucket.test.cpp
    test/gl/context.test.cpp
    test/gl/object.test.cpp

    # include/mbgl
    test/include/mbgl/test.hpp

    # map
    test/map/map.test.cpp
    test/map/prefetch.test.cpp
    test/map/transform.test.cpp

    # math
    test/math/clamp.test.cpp
    test/math/minmax.test.cpp
    test/math/wrap.test.cpp

    # programs
    test/programs/binary_program.test.cpp
    test/programs/symbol_program.test.cpp

    # renderer
    test/renderer/backend_scope.test.cpp
    test/renderer/group_by_layout.test.cpp
    test/renderer/image_manager.test.cpp

    # sprite
    test/sprite/sprite_loader.test.cpp
    test/sprite/sprite_parser.test.cpp

    # src/mbgl/test
    test/src/mbgl/test/fake_file_source.hpp
    test/src/mbgl/test/fixture_log_observer.cpp
    test/src/mbgl/test/fixture_log_observer.hpp
    test/src/mbgl/test/getrss.cpp
    test/src/mbgl/test/getrss.hpp
    test/src/mbgl/test/stub_file_source.cpp
    test/src/mbgl/test/stub_file_source.hpp
    test/src/mbgl/test/stub_geometry_tile_feature.hpp
    test/src/mbgl/test/stub_layer_observer.hpp
    test/src/mbgl/test/stub_render_source_observer.hpp
    test/src/mbgl/test/stub_style_observer.hpp
    test/src/mbgl/test/stub_tile_observer.hpp
    test/src/mbgl/test/test.cpp
    test/src/mbgl/test/util.cpp
    test/src/mbgl/test/util.hpp

    # storage
    test/storage/asset_file_source.test.cpp
    test/storage/default_file_source.test.cpp
    test/storage/headers.test.cpp
    test/storage/http_file_source.test.cpp
    test/storage/local_file_source.test.cpp
    test/storage/offline.test.cpp
    test/storage/offline_database.test.cpp
    test/storage/offline_download.test.cpp
    test/storage/online_file_source.test.cpp
    test/storage/resource.test.cpp
    test/storage/sqlite.test.cpp

    # style/conversion
    test/style/conversion/function.test.cpp
    test/style/conversion/geojson_options.test.cpp
    test/style/conversion/layer.test.cpp
    test/style/conversion/light.test.cpp
    test/style/conversion/stringify.test.cpp
    test/style/conversion/tileset.test.cpp

    # style/expression
    test/style/expression/expression.test.cpp
    test/style/expression/util.test.cpp

    # style
    test/style/filter.test.cpp

    # style/function
    test/style/function/camera_function.test.cpp
    test/style/function/composite_function.test.cpp
    test/style/function/exponential_stops.test.cpp
    test/style/function/interval_stops.test.cpp
    test/style/function/source_function.test.cpp

    # style
    test/style/properties.test.cpp
    test/style/source.test.cpp
    test/style/style.test.cpp
    test/style/style_image.test.cpp
    test/style/style_layer.test.cpp
    test/style/style_parser.test.cpp

    # text
    test/text/cross_tile_symbol_index.test.cpp
    test/text/glyph_manager.test.cpp
    test/text/glyph_pbf.test.cpp
    test/text/local_glyph_rasterizer.test.cpp
    test/text/quads.test.cpp

    # tile
    test/tile/custom_geometry_tile.test.cpp
    test/tile/geojson_tile.test.cpp
    test/tile/geometry_tile_data.test.cpp
    test/tile/raster_tile.test.cpp
    test/tile/tile_coordinate.test.cpp
    test/tile/tile_id.test.cpp
    test/tile/vector_tile.test.cpp

    # util
    test/util/async_task.test.cpp
    test/util/dtoa.test.cpp
    test/util/geo.test.cpp
    test/util/grid_index.test.cpp
    test/util/http_timeout.test.cpp
    test/util/image.test.cpp
    test/util/mapbox.test.cpp
    test/util/memory.test.cpp
    test/util/merge_lines.test.cpp
    test/util/number_conversions.test.cpp
    test/util/offscreen_texture.test.cpp
    test/util/position.test.cpp
    test/util/projection.test.cpp
    test/util/run_loop.test.cpp
    test/util/text_conversions.test.cpp
    test/util/thread.test.cpp
    test/util/thread_local.test.cpp
    test/util/tile_cover.test.cpp
    test/util/timer.test.cpp
    test/util/token.test.cpp
    test/util/unique_any.test.cpp
    test/util/url.test.cpp
)
