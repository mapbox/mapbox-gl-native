#include <mbgl/test/util.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/text/glyph_manager.hpp>

using namespace mbgl;

class RasterTileTest {
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

TEST(RasterTile, setError) {
    RasterTileTest test;
    RasterTile tile(OverscaledTileID(0, 0, 0), test.tileParameters, test.tileset);
    tile.setError(std::make_exception_ptr(std::runtime_error("test")));
    EXPECT_FALSE(tile.isRenderable());
    EXPECT_TRUE(tile.isLoaded());
    EXPECT_TRUE(tile.isComplete());
}

TEST(RasterTile, onError) {
    RasterTileTest test;
    RasterTile tile(OverscaledTileID(0, 0, 0), test.tileParameters, test.tileset);
    tile.onError(std::make_exception_ptr(std::runtime_error("test")));
    EXPECT_FALSE(tile.isRenderable());
    EXPECT_TRUE(tile.isLoaded());
    EXPECT_TRUE(tile.isComplete());
}

TEST(RasterTile, onParsed) {
    RasterTileTest test;
    RasterTile tile(OverscaledTileID(0, 0, 0), test.tileParameters, test.tileset);
    tile.onParsed(std::make_unique<RasterBucket>(PremultipliedImage{}));
    EXPECT_TRUE(tile.isRenderable());
    EXPECT_TRUE(tile.isLoaded());
    EXPECT_TRUE(tile.isComplete());
}

TEST(RasterTile, onParsedEmpty) {
    RasterTileTest test;
    RasterTile tile(OverscaledTileID(0, 0, 0), test.tileParameters, test.tileset);
    tile.onParsed(nullptr);
    EXPECT_FALSE(tile.isRenderable());
    EXPECT_TRUE(tile.isLoaded());
    EXPECT_TRUE(tile.isComplete());
}
