#include <mbgl/test/util.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>

#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/annotation/annotation_manager.hpp>

using namespace mbgl;

class RasterTileTest {
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

TEST(RasterTile, setError) {
    RasterTileTest test;
    RasterTile tile(OverscaledTileID(0, 0, 0), test.updateParameters, test.tileset);
    tile.setError(std::make_exception_ptr(std::runtime_error("test")));
    EXPECT_FALSE(tile.isRenderable());
}

TEST(RasterTile, onError) {
    RasterTileTest test;
    RasterTile tile(OverscaledTileID(0, 0, 0), test.updateParameters, test.tileset);
    tile.onError(std::make_exception_ptr(std::runtime_error("test")));
    EXPECT_TRUE(tile.isRenderable());
}
