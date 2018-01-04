#include <mbgl/test/util.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/test/stub_tile_observer.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>

#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/text/glyph_manager.hpp>

#include <memory>

using namespace mbgl;
using namespace mbgl::style;

class GeoJSONTileTest {
public:
    FakeFileSource fileSource;
    TransformState transformState;
    util::RunLoop loop;
    ThreadPool threadPool { 1 };
    style::Style style { loop, fileSource, 1 };
    AnnotationManager annotationManager { style };
    ImageManager imageManager;
    GlyphManager glyphManager { fileSource };
    Tileset tileset { { "https://example.com" }, { 0, 22 }, "none" };

    TileParameters tileParameters {
        1.0,
        MapDebugOptions(),
        transformState,
        threadPool,
        fileSource,
        MapMode::Continuous,
        annotationManager,
        imageManager,
        glyphManager,
        0
    };
};

TEST(GeoJSONTile, Issue7648) {
    GeoJSONTileTest test;

    CircleLayer layer("circle", "source");

    mapbox::geometry::feature_collection<int16_t> features;
    features.push_back(mapbox::geometry::feature<int16_t> {
        mapbox::geometry::point<int16_t>(0, 0)
    });

    GeoJSONTile tile(OverscaledTileID(0, 0, 0), "source", test.tileParameters, features);

    StubTileObserver observer;
    observer.tileChanged = [&] (const Tile&) {
        // Once present, the bucket should never "disappear", which would cause
        // flickering.
        ASSERT_NE(nullptr, tile.getBucket(*layer.baseImpl));
    };

    tile.setLayers({{ layer.baseImpl }});
    tile.setObserver(&observer);

    while (!tile.isComplete()) {
        test.loop.runOnce();
    }

    tile.updateData(features);
    while (!tile.isComplete()) {
        test.loop.runOnce();
    }
}

// Tests that tiles remain renderable if they have been renderable and then had an error sent to
// them, e.g. when revalidating/refreshing the request.
TEST(GeoJSONTile, Issue9927) {
    GeoJSONTileTest test;

    CircleLayer layer("circle", "source");

    mapbox::geometry::feature_collection<int16_t> features;
    features.push_back(mapbox::geometry::feature<int16_t> {
        mapbox::geometry::point<int16_t>(0, 0)
    });

    GeoJSONTile tile(OverscaledTileID(0, 0, 0), "source", test.tileParameters, features);

    tile.setLayers({{ layer.baseImpl }});

    while (!tile.isComplete()) {
        test.loop.runOnce();
    }

    ASSERT_TRUE(tile.isRenderable());
    ASSERT_NE(nullptr, tile.getBucket(*layer.baseImpl));

    // Make sure that once we've had a renderable tile and then receive erroneous data, we retain
    // the previously rendered data and keep the tile renderable.
    tile.setError(std::make_exception_ptr(std::runtime_error("Connection offline")));
    ASSERT_TRUE(tile.isRenderable());
    ASSERT_NE(nullptr, tile.getBucket(*layer.baseImpl));

    // Then simulate a parsing failure and make sure that we keep it renderable in this situation
    // as well. We're using 3 as a correlationID since we've done two three calls that increment
    // this counter (as part of the GeoJSONTile constructor, setLayers, and setPlacementConfig).
    tile.onError(std::make_exception_ptr(std::runtime_error("Parse error")), 3);
    ASSERT_TRUE(tile.isRenderable());
    ASSERT_NE(nullptr, tile.getBucket(*layer.baseImpl));
 }
