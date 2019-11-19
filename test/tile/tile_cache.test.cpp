#include <mbgl/test/util.hpp>

#include <mbgl/tile/tile_cache.hpp>

#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>
#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/tile/vector_tile_data.hpp>

#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/util/run_loop.hpp>

#include <memory>

using namespace mbgl;

class VectorTileTest {
public:
    std::shared_ptr<FileSource> fileSource = std::make_shared<FakeFileSource>();
    TransformState transformState;
    util::RunLoop loop;
    style::Style style{*fileSource, 1};
    AnnotationManager annotationManager{style};
    ImageManager imageManager;
    GlyphManager glyphManager;
    Tileset tileset{{"https://example.com"}, {0, 22}, "none"};

    TileParameters tileParameters{1.0,
                                  MapDebugOptions(),
                                  transformState,
                                  fileSource,
                                  MapMode::Continuous,
                                  annotationManager,
                                  imageManager,
                                  glyphManager,
                                  0};
};

class VectorTileMock : public VectorTile {
public:
    VectorTileMock(const OverscaledTileID& id,
                   std::string sourceID,
                   const TileParameters& parameters,
                   const Tileset& tileset)
        : VectorTile(id, sourceID, parameters, tileset) {
        renderable = true;
    }
};

TEST(TileCache, Smoke) {
    VectorTileTest test;
    TileCache cache(1);
    OverscaledTileID id(0, 0, 0);
    std::unique_ptr<Tile> tile = std::make_unique<VectorTileMock>(id, "source", test.tileParameters, test.tileset);

    cache.add(id, std::move(tile));
    EXPECT_TRUE(cache.has(id));
    cache.clear();
    EXPECT_FALSE(cache.has(id));
}

TEST(TileCache, Issue15926) {
    VectorTileTest test;
    TileCache cache(2);
    OverscaledTileID id0(0, 0, 0);
    OverscaledTileID id1(1, 0, 0);
    std::unique_ptr<Tile> tile1 = std::make_unique<VectorTileMock>(id0, "source", test.tileParameters, test.tileset);
    std::unique_ptr<Tile> tile2 = std::make_unique<VectorTileMock>(id0, "source", test.tileParameters, test.tileset);
    std::unique_ptr<Tile> tile3 = std::make_unique<VectorTileMock>(id1, "source", test.tileParameters, test.tileset);

    cache.add(id0, std::move(tile1));
    EXPECT_TRUE(cache.has(id0));
    cache.add(id0, std::move(tile2));
    cache.setSize(1);
    cache.add(id1, std::move(tile3));
    EXPECT_FALSE(cache.has(id0));
    EXPECT_TRUE(cache.has(id1));
}
