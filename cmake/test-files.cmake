# Do not edit. Regenerate this with ./scripts/generate-test-files.sh

set(MBGL_TEST_FILES
    # algorithm
    test/algorithm/covered_by_children.cpp
    test/algorithm/generate_clip_ids.cpp
    test/algorithm/mock.hpp
    test/algorithm/update_renderables.cpp

    # api
    test/api/annotations.cpp
    test/api/api_misuse.cpp
    test/api/custom_layer.cpp
    test/api/render_missing.cpp
    test/api/repeated_render.cpp
    test/api/set_style.cpp

    # geometry
    test/geometry/binpack.cpp

    # gl
    test/gl/object.cpp

    # include/mbgl
    test/include/mbgl/test.hpp

    # map
    test/map/map.cpp
    test/map/transform.cpp

    # math
    test/math/clamp.cpp
    test/math/minmax.cpp

    # sprite
    test/sprite/sprite_atlas.cpp
    test/sprite/sprite_image.cpp
    test/sprite/sprite_parser.cpp
    test/sprite/sprite_store.cpp

    # src
    test/src/main.cpp

    # src/mbgl/test
    test/src/mbgl/test/fixture_log_observer.cpp
    test/src/mbgl/test/fixture_log_observer.hpp
    test/src/mbgl/test/stub_file_source.cpp
    test/src/mbgl/test/stub_file_source.hpp
    test/src/mbgl/test/stub_style_observer.hpp
    test/src/mbgl/test/test.cpp
    test/src/mbgl/test/util.cpp
    test/src/mbgl/test/util.hpp

    # storage
    test/storage/asset_file_source.cpp
    test/storage/default_file_source.cpp
    test/storage/headers.cpp
    test/storage/http_file_source.cpp
    test/storage/offline.cpp
    test/storage/offline_database.cpp
    test/storage/offline_download.cpp
    test/storage/online_file_source.cpp
    test/storage/resource.cpp

    # style
    test/style/filter.cpp
    test/style/functions.cpp
    test/style/glyph_store.cpp
    test/style/source.cpp
    test/style/style.cpp
    test/style/style_layer.cpp
    test/style/style_parser.cpp
    test/style/tile_source.cpp
    test/style/variant.cpp

    # text
    test/text/quads.cpp

    # tile
    test/tile/geometry_tile_data.cpp
    test/tile/tile_id.cpp

    # util
    test/util/async_task.cpp
    test/util/geo.cpp
    test/util/image.cpp
    test/util/mapbox.cpp
    test/util/memory.cpp
    test/util/merge_lines.cpp
    test/util/number_conversions.cpp
    test/util/projection.cpp
    test/util/run_loop.cpp
    test/util/text_conversions.cpp
    test/util/thread.cpp
    test/util/thread_local.cpp
    test/util/tile_cover.cpp
    test/util/timer.cpp
    test/util/token.cpp
    test/util/work_queue.cpp
)
