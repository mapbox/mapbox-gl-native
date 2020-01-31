#include <mbgl/test/util.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/test/stub_tile_observer.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>

#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/util/run_loop.hpp>

#include <memory>

using namespace mbgl;
using namespace mbgl::style;

class GeoJSONTileTest {
public:
    std::shared_ptr<FileSource> fileSource = std::make_shared<FakeFileSource>();
    TransformState transformState;
    util::RunLoop loop;
    style::Style style { *fileSource, 1 };
    AnnotationManager annotationManager { style };
    ImageManager imageManager;
    GlyphManager glyphManager;
    Tileset tileset { { "https://example.com" }, { 0, 22 }, "none" };

    TileParameters tileParameters {
        1.0,
        MapDebugOptions(),
        transformState,
        fileSource,
        MapMode::Continuous,
        annotationManager,
        imageManager,
        glyphManager,
        0
    };
};

namespace {

class FakeGeoJSONData : public GeoJSONData {
public:
    FakeGeoJSONData(TileFeatures features_) : features(std::move(features_)) {}

    void getTile(const CanonicalTileID&, const std::function<void(TileFeatures)>& fn) final {
        assert(fn);
        fn(features);
    }

    Features getChildren(const std::uint32_t) final { return {}; }

    Features getLeaves(const std::uint32_t, const std::uint32_t, const std::uint32_t) final { return {}; }

    std::uint8_t getClusterExpansionZoom(std::uint32_t) final { return 0; }

private:
    TileFeatures features;
};

} // namespace

TEST(GeoJSONTile, Issue7648) {
    GeoJSONTileTest test;

    CircleLayer layer("circle", "source");

    mapbox::feature::feature_collection<int16_t> features;
    features.push_back(mapbox::feature::feature<int16_t> { mapbox::geometry::point<int16_t>(0, 0) });
    auto data = std::make_shared<FakeGeoJSONData>(std::move(features));
    GeoJSONTile tile(OverscaledTileID(0, 0, 0), "source", test.tileParameters, data);
    Immutable<LayerProperties> layerProperties = makeMutable<CircleLayerProperties>(staticImmutableCast<CircleLayer::Impl>(layer.baseImpl));
    StubTileObserver observer;
    observer.tileChanged = [&] (const Tile&) {
        // Once present, the bucket should never "disappear", which would cause
        // flickering.
        ASSERT_TRUE(tile.layerPropertiesUpdated(layerProperties));
    };

    std::vector<Immutable<LayerProperties>> layers { layerProperties };
    tile.setLayers(layers);
    tile.setObserver(&observer);

    while (!tile.isComplete()) {
        test.loop.runOnce();
    }

    tile.updateData(data);
    while (!tile.isComplete()) {
        test.loop.runOnce();
    }
}

// Tests that tiles remain renderable if they have been renderable and then had an error sent to
// them, e.g. when revalidating/refreshing the request.
TEST(GeoJSONTile, Issue9927) {
    GeoJSONTileTest test;

    CircleLayer layer("circle", "source");

    mapbox::feature::feature_collection<int16_t> features;
    features.push_back(mapbox::feature::feature<int16_t> { mapbox::geometry::point<int16_t>(0, 0) });
    auto data = std::make_shared<FakeGeoJSONData>(std::move(features));
    GeoJSONTile tile(OverscaledTileID(0, 0, 0), "source", test.tileParameters, data);

    Immutable<LayerProperties> layerProperties = makeMutable<CircleLayerProperties>(staticImmutableCast<CircleLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };
    tile.setLayers(layers);

    while (!tile.isComplete()) {
        test.loop.runOnce();
    }

    ASSERT_TRUE(tile.isRenderable());
    ASSERT_TRUE(tile.layerPropertiesUpdated(layerProperties));

    // Make sure that once we've had a renderable tile and then receive erroneous data, we retain
    // the previously rendered data and keep the tile renderable.
    tile.setError(std::make_exception_ptr(std::runtime_error("Connection offline")));
    ASSERT_TRUE(tile.isRenderable());
    ASSERT_TRUE(tile.layerPropertiesUpdated(layerProperties));

    // Then simulate a parsing failure and make sure that we keep it renderable in this situation
    // as well. We're using 3 as a correlationID since we've done two three calls that increment
    // this counter (as part of the GeoJSONTile constructor, setLayers, and setPlacementConfig).
    tile.onError(std::make_exception_ptr(std::runtime_error("Parse error")), 3);
    ASSERT_TRUE(tile.isRenderable());
    ASSERT_TRUE(tile.layerPropertiesUpdated(layerProperties));
 }
