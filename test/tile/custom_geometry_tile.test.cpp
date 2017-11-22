#include <mbgl/test/util.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/test/stub_tile_observer.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/tile/custom_geometry_tile.hpp>
#include <mbgl/style/custom_tile_loader.hpp>

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

class CustomTileTest {
public:
    FakeFileSource fileSource;
    TransformState transformState;
    util::RunLoop loop;
    ThreadPool threadPool { 1 };
    style::Style style { loop, fileSource, 1 };
    AnnotationManager annotationManager { style };
    ImageManager imageManager;
    GlyphManager glyphManager { fileSource };

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

TEST(CustomGeometryTile, InvokeFetchTile) {
    CustomTileTest test;

    CircleLayer layer("circle", "source");

    mapbox::geometry::feature_collection<double> features;
    features.push_back(mapbox::geometry::feature<double> {
        mapbox::geometry::point<double>(0, 0)
    });
    CustomTileLoader loader([&](const CanonicalTileID& tileId) {
        EXPECT_EQ(tileId, CanonicalTileID(0,0,0));
        test.loop.stop();
    }, [&](const CanonicalTileID&) {

    });
    auto mb =std::make_shared<Mailbox>(*Scheduler::GetCurrent());
    ActorRef<CustomTileLoader> loaderActor(loader, mb);
    
    CustomGeometryTile tile(OverscaledTileID(0, 0, 0), "source", test.tileParameters, CustomGeometrySource::TileOptions(),
    loaderActor);

    tile.setNecessity(TileNecessity::Required);

    test.loop.run();
}

TEST(CustomGeometryTile, InvokeCancelTile) {
    CustomTileTest test;

    CircleLayer layer("circle", "source");

    mapbox::geometry::feature_collection<double> features;
    features.push_back(mapbox::geometry::feature<double> {
        mapbox::geometry::point<double>(0, 0)
    });

    CustomTileLoader loader([&](const CanonicalTileID&) { }, [&](const CanonicalTileID& tileId) {
        EXPECT_EQ(tileId, CanonicalTileID(0,0,0));
        test.loop.stop();
    });
    auto mb =std::make_shared<Mailbox>(*Scheduler::GetCurrent());
    ActorRef<CustomTileLoader> loaderActor(loader, mb);
    
    CustomGeometryTile tile(OverscaledTileID(0, 0, 0), "source", test.tileParameters, CustomGeometrySource::TileOptions(),
    loaderActor);

    tile.setNecessity(TileNecessity::Required);
    tile.setNecessity(TileNecessity::Optional);
    test.loop.run();
}

TEST(CustomGeometryTile, InvokeTileChanged) {
    CustomTileTest test;

    CircleLayer layer("circle", "source");

    mapbox::geometry::feature_collection<double> features;
    features.push_back(mapbox::geometry::feature<double> {
        mapbox::geometry::point<double>(0, 0)
    });

    CustomTileLoader loader(nullptr, nullptr);
    auto mb =std::make_shared<Mailbox>(*Scheduler::GetCurrent());
    ActorRef<CustomTileLoader> loaderActor(loader, mb);
    
    CustomGeometryTile tile(OverscaledTileID(0, 0, 0), "source", test.tileParameters, CustomGeometrySource::TileOptions(),
    loaderActor);

    StubTileObserver observer;
    observer.tileChanged = [&] (const Tile&) {
        // Once present, the bucket should never "disappear", which would cause
        // flickering.
        ASSERT_NE(nullptr, tile.getBucket(*layer.baseImpl));
    };

    tile.setLayers({{ layer.baseImpl }});
    tile.setObserver(&observer);
    tile.setTileData(features);

    while (!tile.isComplete()) {
        test.loop.runOnce();
    }
}
