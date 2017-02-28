#include <mbgl/test/util.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>

#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/annotation/annotation_manager.hpp>

#include <memory>

using namespace mbgl;

class VectorTileTest {
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

TEST(VectorTile, setError) {
    VectorTileTest test;
    VectorTile tile(OverscaledTileID(0, 0, 0), "source", test.updateParameters, test.tileset);
    tile.setError(std::make_exception_ptr(std::runtime_error("test")));
    EXPECT_FALSE(tile.isRenderable());
}

TEST(VectorTile, onError) {
    VectorTileTest test;
    VectorTile tile(OverscaledTileID(0, 0, 0), "source", test.updateParameters, test.tileset);
    tile.onError(std::make_exception_ptr(std::runtime_error("test")));
    EXPECT_TRUE(tile.isRenderable());
}

TEST(VectorTile, Issue7615) {
    VectorTileTest test;
    VectorTile tile(OverscaledTileID(0, 0, 0), "source", test.updateParameters, test.tileset);

    style::SymbolLayer symbolLayer("symbol", "source");
    auto symbolBucket = std::make_shared<SymbolBucket>(
        style::SymbolLayoutProperties::Evaluated(),
        std::unordered_map<
            std::string,
            std::pair<style::IconPaintProperties::Evaluated, style::TextPaintProperties::Evaluated>>(),
        0.0f, false, false);
    
    // Simulate placement of a symbol layer.
    tile.onPlacement(GeometryTile::PlacementResult {
        {{
            symbolLayer.getID(),
            symbolBucket
        }},
        nullptr,
        0
    });

    // Subsequent onLayout should not cause the existing symbol bucket to be discarded.
    tile.onLayout(GeometryTile::LayoutResult {
        {},
        nullptr,
        nullptr,
        0
    });

    EXPECT_EQ(symbolBucket.get(), tile.getBucket(symbolLayer));
}
