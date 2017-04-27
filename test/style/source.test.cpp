#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/stub_style_observer.hpp>
#include <mbgl/test/stub_render_source_observer.hpp>

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>

#include <mbgl/renderer/sources/render_raster_source.hpp>
#include <mbgl/renderer/sources/render_vector_source.hpp>
#include <mbgl/renderer/sources/render_geojson_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/range.hpp>

#include <mbgl/map/transform.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_source.hpp>

#include <mapbox/geojsonvt.hpp>

#include <cstdint>

using namespace mbgl;

class SourceTest {
public:
    util::RunLoop loop;
    StubFileSource fileSource;
    StubStyleObserver styleObserver;
    StubRenderSourceObserver renderSourceObserver;
    Transform transform;
    TransformState transformState;
    ThreadPool threadPool { 1 };
    AnnotationManager annotationManager { 1.0 };
    style::Style style { threadPool, fileSource, 1.0 };

    TileParameters tileParameters {
        1.0,
        MapDebugOptions(),
        transformState,
        threadPool,
        fileSource,
        MapMode::Continuous,
        annotationManager,
        style
    };

    SourceTest() {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        transform.resize({ 512, 512 });
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

    test.styleObserver.sourceError = [&] (Source& source, std::exception_ptr error) {
        EXPECT_EQ("source", source.getID());
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    VectorSource source("source", "url");
    source.baseImpl->setObserver(&test.styleObserver);
    source.baseImpl->loadDescription(test.fileSource);

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

    test.styleObserver.sourceError = [&] (Source& source, std::exception_ptr error) {
        EXPECT_EQ("source", source.getID());
        EXPECT_EQ("0 - Invalid value.", util::toString(error));
        test.end();
    };

    VectorSource source("source", "url");
    source.baseImpl->setObserver(&test.styleObserver);
    source.baseImpl->loadDescription(test.fileSource);

    test.run();
}

TEST(Source, RasterTileEmpty) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.noContent = true;
        return response;
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.baseImpl->loadDescription(test.fileSource);

    test.renderSourceObserver.tileChanged = [&] (RenderSource& source_, const OverscaledTileID&) {
        EXPECT_EQ("source", source_.baseImpl.id);
        test.end();
    };

    test.renderSourceObserver.tileError = [&] (RenderSource&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    RenderRasterSource renderSource(*source.impl);
    renderSource.setObserver(&test.renderSourceObserver);
    renderSource.updateTiles(test.tileParameters);

    test.run();
}

TEST(Source, VectorTileEmpty) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.noContent = true;
        return response;
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.baseImpl->loadDescription(test.fileSource);

    test.renderSourceObserver.tileChanged = [&] (RenderSource& source_, const OverscaledTileID&) {
        EXPECT_EQ("source", source_.baseImpl.id);
        test.end();
    };

    test.renderSourceObserver.tileError = [&] (RenderSource&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    RenderVectorSource renderSource(*source.impl);
    renderSource.setObserver(&test.renderSourceObserver);
    renderSource.updateTiles(test.tileParameters);

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

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.baseImpl->loadDescription(test.fileSource);

    test.renderSourceObserver.tileError = [&] (RenderSource& source_, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(SourceType::Raster, source_.baseImpl.type);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    RenderRasterSource renderSource(*source.impl);
    renderSource.setObserver(&test.renderSourceObserver);
    renderSource.updateTiles(test.tileParameters);

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

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.baseImpl->loadDescription(test.fileSource);

    test.renderSourceObserver.tileError = [&] (RenderSource& source_, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(SourceType::Vector, source_.baseImpl.type);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    RenderVectorSource renderSource(*source.impl);
    renderSource.setObserver(&test.renderSourceObserver);
    renderSource.updateTiles(test.tileParameters);

    test.run();
}

TEST(Source, RasterTileCorrupt) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.baseImpl->loadDescription(test.fileSource);

    test.renderSourceObserver.tileError = [&] (RenderSource& source_, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source_.baseImpl.type, SourceType::Raster);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_TRUE(bool(error));
        // Not asserting on platform-specific error text.
        test.end();
    };

    RenderRasterSource renderSource(*source.impl);
    renderSource.setObserver(&test.renderSourceObserver);
    renderSource.updateTiles(test.tileParameters);

    test.run();
}

TEST(Source, VectorTileCorrupt) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    // Need to have at least one layer that uses the source.
    auto layer = std::make_unique<LineLayer>("id", "source");
    layer->setSourceLayer("water");
    test.style.addLayer(std::move(layer));

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.baseImpl->loadDescription(test.fileSource);

    test.renderSourceObserver.tileError = [&] (RenderSource& source_, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source_.baseImpl.type, SourceType::Vector);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_EQ(util::toString(error), "unknown pbf field type exception");
        test.end();
    };

    RenderVectorSource renderSource(*source.impl);
    renderSource.setObserver(&test.renderSourceObserver);
    renderSource.updateTiles(test.tileParameters);

    test.run();
}

TEST(Source, RasterTileCancel) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.baseImpl->loadDescription(test.fileSource);

    test.renderSourceObserver.tileChanged = [&] (RenderSource&, const OverscaledTileID&) {
        FAIL() << "Should never be called";
    };

    test.renderSourceObserver.tileError = [&] (RenderSource&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    RenderRasterSource renderSource(*source.impl);
    renderSource.setObserver(&test.renderSourceObserver);
    renderSource.updateTiles(test.tileParameters);

    test.run();
}

TEST(Source, VectorTileCancel) {
    SourceTest test;

    test.fileSource.tileResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.baseImpl->loadDescription(test.fileSource);

    test.renderSourceObserver.tileChanged = [&] (RenderSource&, const OverscaledTileID&) {
        FAIL() << "Should never be called";
    };

    test.renderSourceObserver.tileError = [&] (RenderSource&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    RenderVectorSource renderSource(*source.impl);
    renderSource.setObserver(&test.renderSourceObserver);
    renderSource.updateTiles(test.tileParameters);

    test.run();
}

TEST(Source, RasterTileAttribution) {
    SourceTest test;

    std::string mapboxOSM = ("<a href='https://www.mapbox.com/about/maps/' target='_blank'>&copy; Mapbox</a> "
                             "<a href='http://www.openstreetmap.org/about/' target='_blank'>©️ OpenStreetMap</a>");

    test.fileSource.tileResponse = [&] (const Resource&) {
        Response response;
        response.noContent = true;
        return response;
    };

    test.fileSource.sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("url", resource.url);
        Response response;
        response.data = std::make_unique<std::string>(R"TILEJSON({ "tilejson": "2.1.0", "attribution": ")TILEJSON" +
                                                      mapboxOSM +
                                                      R"TILEJSON(", "tiles": [ "tiles" ] })TILEJSON");
        return response;
    };

    test.styleObserver.sourceChanged = [&] (Source& source) {
        EXPECT_EQ(mapboxOSM, source.getAttribution());
        EXPECT_FALSE(mapboxOSM.find("©️ OpenStreetMap") == std::string::npos);
        test.end();
    };

    RasterSource source("source", "url", 512);
    source.baseImpl->setObserver(&test.styleObserver);
    source.baseImpl->loadDescription(test.fileSource);

    RenderRasterSource renderSource(*source.impl);
    renderSource.updateTiles(test.tileParameters);

    test.run();
}

TEST(Source, GeoJSonSourceUrlUpdate) {
    SourceTest test;

    test.fileSource.sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("url", resource.url);
        Response response;
        response.data = std::make_unique<std::string>("{\"geometry\": {\"type\": \"Point\", \"coordinates\": [1.1, 1.1]}, \"type\": \"Feature\", \"properties\": {}}");
        return response;
    };

    test.styleObserver.sourceDescriptionChanged = [&] (Source&) {
        // Should be called (test will hang if it doesn't)
        test.end();
    };

    GeoJSONSource source("source");
    source.baseImpl->setObserver(&test.styleObserver);

    // Load initial, so the source state will be loaded=true
    source.baseImpl->loadDescription(test.fileSource);

    // Schedule an update
    test.loop.invoke([&] () {
        // Update the url
        source.setURL(std::string("http://source-url.ext"));
    });

    test.run();
}
