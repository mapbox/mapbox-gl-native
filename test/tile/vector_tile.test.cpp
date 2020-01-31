#include <mbgl/test/util.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/tile/vector_tile_data.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/text/glyph_manager.hpp>

#include <memory>

using namespace mbgl;

class VectorTileTest {
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

TEST(VectorTile, setError) {
    VectorTileTest test;
    VectorTile tile(OverscaledTileID(0, 0, 0), "source", test.tileParameters, test.tileset);
    tile.setError(std::make_exception_ptr(std::runtime_error("test")));
    EXPECT_FALSE(tile.isRenderable());
    EXPECT_TRUE(tile.isLoaded());
    EXPECT_TRUE(tile.isComplete());
}

TEST(VectorTile, onError) {
    VectorTileTest test;
    VectorTile tile(OverscaledTileID(0, 0, 0), "source", test.tileParameters, test.tileset);
    tile.onError(std::make_exception_ptr(std::runtime_error("test")), 0);

    EXPECT_FALSE(tile.isRenderable());
    EXPECT_TRUE(tile.isLoaded());
    EXPECT_TRUE(tile.isComplete());
}

TEST(VectorTile, Issue8542) {
    VectorTileTest test;
    VectorTile tile(OverscaledTileID(0, 0, 0), "source", test.tileParameters, test.tileset);

    // Query before data is set
    std::vector<Feature> result;
    tile.querySourceFeatures(result, { { {"layer"} }, {} });
}

TEST(VectorTileData, ParseResults) {
    VectorTileData data(std::make_shared<std::string>(util::read_file("test/fixtures/map/issue12432/0-0-0.mvt")));

    std::vector<std::string> layerNames = data.layerNames();
    ASSERT_EQ(layerNames.size(), 2u);
    ASSERT_EQ(layerNames.at(0), "admin");
    ASSERT_EQ(layerNames.at(1), "water");

    ASSERT_FALSE(data.getLayer("invalid"));

    std::unique_ptr<GeometryTileLayer> layer = data.getLayer("admin");
    ASSERT_EQ(layer->getName(), "admin");
    ASSERT_EQ(layer->featureCount(), 17154u);

    try {
        layer->getFeature(17154u);
        ASSERT_TRUE(false) << "should throw: feature index is out of range.";
    } catch (const std::out_of_range&) {
        ASSERT_TRUE(true);
    }

    std::unique_ptr<GeometryTileFeature> feature = layer->getFeature(0u);
    ASSERT_EQ(feature->getType(), mbgl::FeatureType::LineString);
    ASSERT_TRUE(feature->getID().is<uint64_t>());
    ASSERT_EQ(feature->getID().get<uint64_t>(), 1u);

    const std::unordered_map<std::string, Value>& properties = feature->getProperties();
    ASSERT_EQ(properties.size(), 3u);
    ASSERT_EQ(properties.at("disputed"), *feature->getValue("disputed"));

    ASSERT_EQ(feature->getValue("invalid"), nullopt);
}
