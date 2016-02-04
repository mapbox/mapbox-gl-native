#include "../fixtures/util.hpp"
#include "../fixtures/stub_file_source.hpp"
#include "../fixtures/mock_view.hpp"
#include "../fixtures/stub_style_observer.hpp"

#include <mbgl/map/source.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/platform/log.hpp>

#include <mbgl/map/transform.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_update_parameters.hpp>
#include <mbgl/layer/line_layer.hpp>

using namespace mbgl;

class SourceTest {
public:
    util::ThreadContext context { "Map", util::ThreadType::Map, util::ThreadPriority::Regular };
    util::RunLoop loop;
    StubFileSource fileSource;
    StubStyleObserver observer;
    MockView view;
    Transform transform { view, ConstrainMode::HeightOnly };
    TransformState transformState;
    Worker worker { 1 };
    TexturePool texturePool;
    MapData mapData { MapMode::Still, GLContextMode::Unique, 1.0 };
    Style style { mapData };

    StyleUpdateParameters updateParameters {
        1.0,
        MapDebugOptions(),
        TimePoint(),
        transformState,
        worker,
        texturePool,
        true,
        MapMode::Continuous,
        mapData,
        style
    };

    SourceTest() {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        util::ThreadContext::Set(&context);
        util::ThreadContext::setFileSource(&fileSource);

        transform.resize({{ 512, 512 }});
        transform.setLatLngZoom({0, 0}, 0);

        transformState = transform.getState();
    }

    void run() {
        loop.run();
    }

    void end() {
        loop.stop();
    }
};

TEST(Source, LoadingFail) {
    SourceTest test;

    test.fileSource.sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("url", resource.url);
        Response response;
        response.error = std::make_unique<Response::Error>(
            Response::Error::Reason::Other,
            "Failed by the test case");
        return response;
    };

    test.observer.sourceError = [&] (Source& source, std::exception_ptr error) {
        EXPECT_EQ("url", source.url);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    Source source(SourceType::Vector, "source", "url", 512, nullptr, nullptr);
    source.setObserver(&test.observer);
    source.load();

    test.run();
}

TEST(Source, LoadingCorrupt) {
    SourceTest test;

    test.fileSource.sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("url", resource.url);
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    test.observer.sourceError = [&] (Source& source, std::exception_ptr error) {
        EXPECT_EQ("url", source.url);
        EXPECT_EQ("0 - Invalid value.", util::toString(error));
        test.end();
    };

    Source source(SourceType::Vector, "source", "url", 512, nullptr, nullptr);
    source.setObserver(&test.observer);
    source.load();

    test.run();
}

TEST(Source, RasterTileFail) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.error = std::make_unique<Response::Error>(
            Response::Error::Reason::Other,
            "Failed by the test case");
        return response;
    };

    test.observer.tileError = [&] (Source& source, const TileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(SourceType::Raster, source.type);
        EXPECT_EQ("0/0/0", std::string(tileID));
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    auto info = std::make_unique<SourceInfo>();
    info->tiles = { "tiles" };

    Source source(SourceType::Raster, "source", "", 512, std::move(info), nullptr);
    source.setObserver(&test.observer);
    source.load();
    source.update(test.updateParameters);

    test.run();
}

TEST(Source, VectorTileFail) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.error = std::make_unique<Response::Error>(
            Response::Error::Reason::Other,
            "Failed by the test case");
        return response;
    };

    test.observer.tileError = [&] (Source& source, const TileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(SourceType::Vector, source.type);
        EXPECT_EQ("0/0/0", std::string(tileID));
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    auto info = std::make_unique<SourceInfo>();
    info->tiles = { "tiles" };

    Source source(SourceType::Vector, "source", "", 512, std::move(info), nullptr);
    source.setObserver(&test.observer);
    source.load();
    source.update(test.updateParameters);

    test.run();
}

TEST(Source, RasterTileCorrupt) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    test.observer.tileError = [&] (Source& source, const TileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source.type, SourceType::Raster);
        EXPECT_EQ(std::string(tileID), "0/0/0");
        EXPECT_TRUE(bool(error));
        // Not asserting on platform-specific error text.
        test.end();
    };

    auto info = std::make_unique<SourceInfo>();
    info->tiles = { "tiles" };

    Source source(SourceType::Raster, "source", "", 512, std::move(info), nullptr);
    source.setObserver(&test.observer);
    source.load();
    source.update(test.updateParameters);

    test.run();
}

TEST(Source, VectorTileCorrupt) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    test.observer.tileError = [&] (Source& source, const TileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source.type, SourceType::Vector);
        EXPECT_EQ(std::string(tileID), "0/0/0");
        EXPECT_EQ(util::toString(error), "pbf unknown field type exception");
        test.end();
    };

    // Need to have at least one layer that uses the source.
    auto layer = std::make_unique<LineLayer>();
    layer->source = "source";
    layer->sourceLayer = "water";
    test.style.addLayer(std::move(layer));

    auto info = std::make_unique<SourceInfo>();
    info->tiles = { "tiles" };

    Source source(SourceType::Vector, "source", "", 512, std::move(info), nullptr);
    source.setObserver(&test.observer);
    source.load();
    source.update(test.updateParameters);

    test.run();
}

TEST(Source, RasterTileCancel) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    test.observer.tileLoaded = [&] (Source&, const TileID&, bool) {
        FAIL() << "Should never be called";
    };

    test.observer.tileError = [&] (Source&, const TileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    auto info = std::make_unique<SourceInfo>();
    info->tiles = { "tiles" };

    Source source(SourceType::Raster, "source", "", 512, std::move(info), nullptr);
    source.setObserver(&test.observer);
    source.load();
    source.update(test.updateParameters);

    test.run();
}

TEST(Source, VectorTileCancel) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    test.observer.tileLoaded = [&] (Source&, const TileID&, bool) {
        FAIL() << "Should never be called";
    };

    test.observer.tileError = [&] (Source&, const TileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    auto info = std::make_unique<SourceInfo>();
    info->tiles = { "tiles" };

    Source source(SourceType::Vector, "source", "", 512, std::move(info), nullptr);
    source.setObserver(&test.observer);
    source.load();
    source.update(test.updateParameters);

    test.run();
}
