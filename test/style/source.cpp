#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/stub_style_observer.hpp>

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/platform/log.hpp>

#include <mbgl/map/transform.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/annotation/annotation_manager.hpp>

#include <mapbox/geojsonvt.hpp>

using namespace mbgl;

class SourceTest {
public:
    util::RunLoop loop;
    StubFileSource fileSource;
    StubStyleObserver observer;
    Transform transform;
    TransformState transformState;
    Worker worker { 1 };
    AnnotationManager annotationManager { 1.0 };
    style::Style style { fileSource, 1.0 };

    style::UpdateParameters updateParameters {
        1.0,
        MapDebugOptions(),
        TimePoint(),
        transformState,
        worker,
        fileSource,
        true,
        MapMode::Continuous,
        annotationManager,
        style
    };

    SourceTest() {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

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
        EXPECT_EQ("source", source.getID());
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    VectorSource source("source", "url");
    source.baseImpl->setObserver(&test.observer);
    source.baseImpl->load(test.fileSource);

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
        EXPECT_EQ("source", source.getID());
        EXPECT_EQ("0 - Invalid value.", util::toString(error));
        test.end();
    };

    VectorSource source("source", "url");
    source.baseImpl->setObserver(&test.observer);
    source.baseImpl->load(test.fileSource);

    test.run();
}

TEST(Source, RasterTileEmpty) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.noContent = true;
        return response;
    };

    test.observer.tileLoaded = [&] (Source& source, const OverscaledTileID&, bool) {
        EXPECT_EQ("source", source.getID());
        test.end();
    };

    test.observer.tileError = [&] (Source&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.baseImpl->setObserver(&test.observer);
    source.baseImpl->load(test.fileSource);
    source.baseImpl->update(test.updateParameters);

    test.run();
}

TEST(Source, VectorTileEmpty) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.noContent = true;
        return response;
    };

    test.observer.tileLoaded = [&] (Source& source, const OverscaledTileID&, bool) {
        EXPECT_EQ("source", source.getID());
        test.end();
    };

    test.observer.tileError = [&] (Source&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.baseImpl->setObserver(&test.observer);
    source.baseImpl->load(test.fileSource);
    source.baseImpl->update(test.updateParameters);

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

    test.observer.tileError = [&] (Source& source, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(SourceType::Raster, source.baseImpl->type);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.baseImpl->setObserver(&test.observer);
    source.baseImpl->load(test.fileSource);
    source.baseImpl->update(test.updateParameters);

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

    test.observer.tileError = [&] (Source& source, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(SourceType::Vector, source.baseImpl->type);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.baseImpl->setObserver(&test.observer);
    source.baseImpl->load(test.fileSource);
    source.baseImpl->update(test.updateParameters);

    test.run();
}

TEST(Source, RasterTileCorrupt) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    test.observer.tileError = [&] (Source& source, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source.baseImpl->type, SourceType::Raster);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_TRUE(bool(error));
        // Not asserting on platform-specific error text.
        test.end();
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.baseImpl->setObserver(&test.observer);
    source.baseImpl->load(test.fileSource);
    source.baseImpl->update(test.updateParameters);

    test.run();
}

TEST(Source, VectorTileCorrupt) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    test.observer.tileError = [&] (Source& source, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source.baseImpl->type, SourceType::Vector);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_EQ(util::toString(error), "unknown pbf field type exception");
        test.end();
    };

    // Need to have at least one layer that uses the source.
    auto layer = std::make_unique<LineLayer>("id", "source");
    layer->setSourceLayer("water");
    test.style.addLayer(std::move(layer));

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.baseImpl->setObserver(&test.observer);
    source.baseImpl->load(test.fileSource);
    source.baseImpl->update(test.updateParameters);

    test.run();
}

TEST(Source, RasterTileCancel) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    test.observer.tileLoaded = [&] (Source&, const OverscaledTileID&, bool) {
        FAIL() << "Should never be called";
    };

    test.observer.tileError = [&] (Source&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.baseImpl->setObserver(&test.observer);
    source.baseImpl->load(test.fileSource);
    source.baseImpl->update(test.updateParameters);

    test.run();
}

TEST(Source, VectorTileCancel) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    test.observer.tileLoaded = [&] (Source&, const OverscaledTileID&, bool) {
        FAIL() << "Should never be called";
    };

    test.observer.tileError = [&] (Source&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.baseImpl->setObserver(&test.observer);
    source.baseImpl->load(test.fileSource);
    source.baseImpl->update(test.updateParameters);

    test.run();
}
