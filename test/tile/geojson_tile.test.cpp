#include <mbgl/test/util.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/test/stub_tile_observer.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>

#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/annotation/annotation_manager.hpp>

#include <memory>

using namespace mbgl;
using namespace mbgl::style;

class GeoJSONTileTest {
public:
    FakeFileSource fileSource;
    TransformState transformState;
    util::RunLoop loop;
    ThreadPool threadPool { 1 };
    AnnotationManager annotationManager { 1.0 };
    style::Style style { fileSource, 1.0 };
    Tileset tileset { { "https://example.com" }, { 0, 22 }, "none" };

    style::UpdateParameters updateParameters {
        1.0,
        MapDebugOptions(),
        transformState,
        threadPool,
        fileSource,
        MapMode::Continuous,
        annotationManager,
        style
    };
};

TEST(GeoJSONTile, Issue7648) {
    GeoJSONTileTest test;
    GeoJSONTile tile(OverscaledTileID(0, 0, 0), "source", test.updateParameters);

    test.style.addLayer(std::make_unique<CircleLayer>("circle", "source"));

    StubTileObserver observer;
    observer.tileChanged = [&] (const Tile&) {
        // Once present, the bucket should never "disappear", which would cause
        // flickering.
        ASSERT_NE(nullptr, tile.getBucket(*test.style.getLayer("circle")));
    };
    tile.setObserver(&observer);

    tile.setPlacementConfig({});

    mapbox::geometry::feature_collection<int16_t> features;
    features.push_back(mapbox::geometry::feature<int16_t> {
        mapbox::geometry::point<int16_t>(0, 0)
    });

    tile.updateData(features);
    while (!tile.isComplete()) {
        test.loop.runOnce();
    }

    tile.updateData(features);
    while (!tile.isComplete()) {
        test.loop.runOnce();
    }
}
