#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/stub_render_source_observer.hpp>
#include <mbgl/test/stub_style_observer.hpp>
#include <mbgl/test/util.hpp>

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/hillshade_layer_impl.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/style/sources/raster_dem_source.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/style.hpp>

#include <mbgl/renderer/sources/render_raster_source.hpp>
#include <mbgl/renderer/sources/render_raster_dem_source.hpp>
#include <mbgl/renderer/sources/render_vector_source.hpp>
#include <mbgl/renderer/sources/render_geojson_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/premultiply.hpp>
#include <mbgl/util/image.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/range.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/timer.hpp>

#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/tile_render_data.hpp>
#include <mbgl/text/glyph_manager.hpp>

#include <cstdint>
#include <gmock/gmock.h>

using namespace mbgl;
using SourceType = mbgl::style::SourceType;

class SourceTest {
public:
    util::RunLoop loop;
    std::shared_ptr<StubFileSource> fileSource = std::make_shared<StubFileSource>();
    StubStyleObserver styleObserver;
    StubRenderSourceObserver renderSourceObserver;
    Transform transform;
    TransformState transformState;
    Style style{fileSource, 1};
    AnnotationManager annotationManager { style };
    ImageManager imageManager;
    GlyphManager glyphManager;

    TileParameters tileParameters(MapMode mapMode = MapMode::Continuous) {
        return {1.0,
                MapDebugOptions(),
                transformState,
                fileSource,
                mapMode,
                annotationManager.makeWeakPtr(),
                imageManager,
                glyphManager,
                0};
    };

    SourceTest() {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        transform.resize({ 512, 512 });
        transform.jumpTo(CameraOptions().withCenter(LatLng()).withZoom(0.0));

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

    test.fileSource->sourceResponse = [&] (const Resource& resource) {
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
    source.setObserver(&test.styleObserver);
    source.loadDescription(*test.fileSource);

    test.run();
}

TEST(Source, LoadingCorrupt) {
    SourceTest test;

    test.fileSource->sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("url", resource.url);
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    test.styleObserver.sourceError = [&] (Source& source, std::exception_ptr error) {
        EXPECT_EQ("source", source.getID());
        EXPECT_EQ("Invalid value. at offset 0", util::toString(error));
        test.end();
    };

    VectorSource source("source", "url");
    source.setObserver(&test.styleObserver);
    source.loadDescription(*test.fileSource);

    test.run();
}

TEST(Source, RasterTileEmpty) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        Response response;
        response.noContent = true;
        return response;
    };

    RasterLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties = makeMutable<RasterLayerProperties>(staticImmutableCast<RasterLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileChanged = [&] (RenderSource& source_, const OverscaledTileID&) {
        EXPECT_EQ("source", source_.baseImpl->id);
        test.end();
    };

    test.renderSourceObserver.tileError = [&] (RenderSource&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, RasterDEMTileEmpty) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        Response response;
        response.noContent = true;
        return response;
    };

    HillshadeLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties = makeMutable<HillshadeLayerProperties>(staticImmutableCast<HillshadeLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterDEMSource source("source", tileset, 512);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileChanged = [&] (RenderSource& source_, const OverscaledTileID&) {
        EXPECT_EQ("source", source_.baseImpl->id);
        test.end();
    };

    test.renderSourceObserver.tileError = [&] (RenderSource&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, VectorTileEmpty) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        Response response;
        response.noContent = true;
        return response;
    };

    LineLayer layer("id", "source");
    layer.setSourceLayer("water");

    Immutable<LayerProperties> layerProperties = makeMutable<LineLayerProperties>(staticImmutableCast<LineLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileChanged = [&] (RenderSource& source_, const OverscaledTileID&) {
        EXPECT_EQ("source", source_.baseImpl->id);
        test.end();
    };

    test.renderSourceObserver.tileError = [&] (RenderSource&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, RasterTileFail) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        Response response;
        response.error = std::make_unique<Response::Error>(
            Response::Error::Reason::Other,
            "Failed by the test case");
        return response;
    };

    RasterLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties = makeMutable<RasterLayerProperties>(staticImmutableCast<RasterLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileError = [&] (RenderSource& source_, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(SourceType::Raster, source_.baseImpl->type);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, RasterDEMTileFail) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        Response response;
        response.error = std::make_unique<Response::Error>(
            Response::Error::Reason::Other,
            "Failed by the test case");
        return response;
    };

    HillshadeLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties = makeMutable<HillshadeLayerProperties>(staticImmutableCast<HillshadeLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterDEMSource source("source", tileset, 512);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileError = [&] (RenderSource& source_, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(SourceType::RasterDEM, source_.baseImpl->type);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, VectorTileFail) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        Response response;
        response.error = std::make_unique<Response::Error>(
            Response::Error::Reason::Other,
            "Failed by the test case");
        return response;
    };

    LineLayer layer("id", "source");
    layer.setSourceLayer("water");

    Immutable<LayerProperties> layerProperties = makeMutable<LineLayerProperties>(staticImmutableCast<LineLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileError = [&] (RenderSource& source_, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(SourceType::Vector, source_.baseImpl->type);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, RasterTileCorrupt) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    RasterLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties = makeMutable<RasterLayerProperties>(staticImmutableCast<RasterLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileError = [&] (RenderSource& source_, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source_.baseImpl->type, SourceType::Raster);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_TRUE(bool(error));
        // Not asserting on platform-specific error text.
        test.end();
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, RasterDEMTileCorrupt) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    HillshadeLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties = makeMutable<HillshadeLayerProperties>(staticImmutableCast<HillshadeLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };;

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterDEMSource source("source", tileset, 512);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileError = [&] (RenderSource& source_, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source_.baseImpl->type, SourceType::RasterDEM);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_TRUE(bool(error));
        // Not asserting on platform-specific error text.
        test.end();
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, VectorTileCorrupt) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    LineLayer layer("id", "source");
    layer.setSourceLayer("water");

    Immutable<LayerProperties> layerProperties = makeMutable<LineLayerProperties>(staticImmutableCast<LineLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileError = [&] (RenderSource& source_, const OverscaledTileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source_.baseImpl->type, SourceType::Vector);
        EXPECT_EQ(OverscaledTileID(0, 0, 0), tileID);
        EXPECT_EQ(util::toString(error), "unknown pbf field type exception");
        test.end();
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, RasterTileCancel) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    RasterLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties = makeMutable<RasterLayerProperties>(staticImmutableCast<RasterLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterSource source("source", tileset, 512);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileChanged = [&] (RenderSource&, const OverscaledTileID&) {
        FAIL() << "Should never be called";
    };

    test.renderSourceObserver.tileError = [&] (RenderSource&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, RasterDEMTileCancel) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    HillshadeLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties = makeMutable<HillshadeLayerProperties>(staticImmutableCast<HillshadeLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    RasterDEMSource source("source", tileset, 512);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileChanged = [&] (RenderSource&, const OverscaledTileID&) {
        FAIL() << "Should never be called";
    };

    test.renderSourceObserver.tileError = [&] (RenderSource&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, VectorTileCancel) {
    SourceTest test;

    test.fileSource->tileResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    LineLayer layer("id", "source");
    layer.setSourceLayer("water");

    Immutable<LayerProperties> layerProperties = makeMutable<LineLayerProperties>(staticImmutableCast<LineLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    Tileset tileset;
    tileset.tiles = { "tiles" };

    VectorSource source("source", tileset);
    source.loadDescription(*test.fileSource);

    test.renderSourceObserver.tileChanged = [&] (RenderSource&, const OverscaledTileID&) {
        FAIL() << "Should never be called";
    };

    test.renderSourceObserver.tileError = [&] (RenderSource&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, RasterTileAttribution) {
    SourceTest test;

    RasterLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties = makeMutable<RasterLayerProperties>(staticImmutableCast<RasterLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    std::string mapboxOSM = ("<a href='https://www.mapbox.com/about/maps/' target='_blank'>&copy; Mapbox</a> "
                             "<a href='http://www.openstreetmap.org/about/' target='_blank'>©️ OpenStreetMap</a>");

    test.fileSource->tileResponse = [&] (const Resource&) {
        Response response;
        response.noContent = true;
        return response;
    };

    test.fileSource->sourceResponse = [&] (const Resource& resource) {
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
    source.setObserver(&test.styleObserver);
    source.loadDescription(*test.fileSource);

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, RasterDEMTileAttribution) {
    SourceTest test;

    HillshadeLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties = makeMutable<HillshadeLayerProperties>(staticImmutableCast<HillshadeLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    std::string mapbox = ("<a href='https://www.mapbox.com/about/maps/' target='_blank'>&copy; Mapbox</a> ");

    test.fileSource->tileResponse = [&] (const Resource&) {
        Response response;
        response.noContent = true;
        return response;
    };

    test.fileSource->sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("url", resource.url);
        Response response;
        response.data = std::make_unique<std::string>(R"TILEJSON({ "tilejson": "2.1.0", "attribution": ")TILEJSON" +
                                                      mapbox +
                                                      R"TILEJSON(", "tiles": [ "tiles" ] })TILEJSON");
        return response;
    };

    test.styleObserver.sourceChanged = [&] (Source& source) {
        EXPECT_EQ(mapbox, source.getAttribution());
        test.end();
    };

    RasterDEMSource source("source", "url", 512);
    source.setObserver(&test.styleObserver);
    source.loadDescription(*test.fileSource);

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.run();
}

TEST(Source, GeoJSonSourceUrlUpdate) {
    SourceTest test;

    test.fileSource->sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("url", resource.url);
        Response response;
        response.data = std::make_unique<std::string>(R"({"geometry": {"type": "Point", "coordinates": [1.1, 1.1]}, "type": "Feature", "properties": {}})");
        return response;
    };

    test.styleObserver.sourceDescriptionChanged = [&] (Source&) {
        // Should be called (test will hang if it doesn't)
        test.end();
    };

    GeoJSONSource source("source");
    source.setObserver(&test.styleObserver);

    // Load initial, so the source state will be loaded=true
    source.loadDescription(*test.fileSource);

    // Schedule an update
    test.loop.invoke([&] () {
        // Update the url
        source.setURL(std::string("http://source-url.ext"));
    });

    test.run();
}

TEST(Source, ImageSourceImageUpdate) {
    SourceTest test;

    test.fileSource->response = [&] (const Resource& resource) {
        EXPECT_EQ("http://url", resource.url);
        Response response;
        response.data = std::make_unique<std::string>(util::read_file("test/fixtures/image/no_profile.png"));
        return response;
    };
    test.styleObserver.sourceChanged = [&] (Source&) {
        // Should be called (test will hang if it doesn't)
        test.end();
    };
    std::array<LatLng, 4> coords;

    ImageSource source("source", coords);
    source.setURL("http://url");
    source.setObserver(&test.styleObserver);

    // Load initial, so the source state will be loaded=true
    source.loadDescription(*test.fileSource);
    PremultipliedImage rgba({ 1, 1 });
    rgba.data[0] = 255;
    rgba.data[1] = 254;
    rgba.data[2] = 253;
    rgba.data[3] = 0;

    // Schedule an update
    test.loop.invoke([&] () {
        // Update the url
        source.setImage(std::move(rgba));
    });

    test.run();
}

TEST(Source, CustomGeometrySourceSetTileData) {
    SourceTest test;
    CustomGeometrySource source("source", CustomGeometrySource::Options());
    source.loadDescription(*test.fileSource);

    LineLayer layer("id", "source");
    layer.setSourceLayer("water");
    Immutable<LayerProperties> layerProperties = makeMutable<LineLayerProperties>(staticImmutableCast<LineLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    test.renderSourceObserver.tileChanged = [&] (RenderSource& source_, const OverscaledTileID&) {
        EXPECT_EQ("source", source_.baseImpl->id);
        test.end();
    };

    test.renderSourceObserver.tileError = [&] (RenderSource&, const OverscaledTileID&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.loop.invoke([&] () {
        // Set Tile Data
        source.setTileData(CanonicalTileID(0, 0, 0), GeoJSON{ FeatureCollection{} });
    });

    test.run();
}
namespace {

class FakeTileSource;

class FakeTile : public Tile {
public:
    FakeTile(FakeTileSource& source_, const OverscaledTileID& tileID)
        : Tile(Tile::Kind::Geometry, tileID), source(source_) {
        renderable = true;
    }
    void setNecessity(TileNecessity necessity) override;
    bool layerPropertiesUpdated(const Immutable<style::LayerProperties>&) override { return true; }

    std::unique_ptr<TileRenderData> createRenderData() override { return nullptr; }

private:
    FakeTileSource& source;
};

class FakeTileSource : public RenderTileSetSource {
public:
    MOCK_METHOD1(tileSetNecessity, void(TileNecessity));

    explicit FakeTileSource(Immutable<style::Source::Impl> impl_) : RenderTileSetSource(std::move(impl_)) {}
    void updateInternal(const Tileset& tileset,
                        const std::vector<Immutable<style::LayerProperties>>& layers,
                        const bool needsRendering,
                        const bool needsRelayout,
                        const TileParameters& parameters) override {
        tilePyramid.update(layers,
                           needsRendering,
                           needsRelayout,
                           parameters,
                           *baseImpl,
                           util::tileSize,
                           tileset.zoomRange,
                           tileset.bounds,
                           [&](const OverscaledTileID& tileID) { return std::make_unique<FakeTile>(*this, tileID); });
    }

    const optional<Tileset>& getTileset() const override {
        return static_cast<const style::VectorSource::Impl&>(*baseImpl).tileset;
    }
};

void FakeTile::setNecessity(TileNecessity necessity) {
    source.tileSetNecessity(necessity);
}

} // namespace

TEST(Source, InvisibleSourcesTileNecessity) {
    SourceTest test;
    VectorSource initialized("source", Tileset{{"tiles"}});
    initialized.loadDescription(*test.fileSource);

    FakeTileSource renderTilesetSource{initialized.baseImpl};
    RenderSource* renderSource = &renderTilesetSource;
    LineLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties =
        makeMutable<LineLayerProperties>(staticImmutableCast<LineLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers{layerProperties};
    EXPECT_CALL(renderTilesetSource, tileSetNecessity(TileNecessity::Required)).Times(1);
    renderSource->update(initialized.baseImpl, layers, true, true, test.tileParameters());

    // Necessity for invisible tiles must be set to `optional`.
    EXPECT_CALL(renderTilesetSource, tileSetNecessity(TileNecessity::Optional)).Times(1);
    renderSource->update(initialized.baseImpl, layers, false, false, test.tileParameters());

    // Necessity is again `required` once tiles get back visible.
    EXPECT_CALL(renderTilesetSource, tileSetNecessity(TileNecessity::Required)).Times(1);
    renderSource->update(initialized.baseImpl, layers, true, false, test.tileParameters());
}

TEST(Source, RenderTileSetSourceUpdate) {
    SourceTest test;

    class FakeRenderTileSetSource : public RenderTileSetSource {
    public:
        explicit FakeRenderTileSetSource(Immutable<style::Source::Impl> impl_)
            : RenderTileSetSource(std::move(impl_)) {}

        MOCK_METHOD0(mockedUpdateInternal, void());

        void updateInternal(const Tileset&,
                            const std::vector<Immutable<style::LayerProperties>>&,
                            const bool, const bool, const TileParameters&) override {
            mockedUpdateInternal();
        }

        const optional<Tileset>& getTileset() const override {
            return static_cast<const style::VectorSource::Impl&>(*baseImpl).tileset;
        }
    };

    VectorSource initialized("source", Tileset{ {"tiles"} });
    initialized.loadDescription(*test.fileSource);

    FakeRenderTileSetSource renderTilesetSource { initialized.baseImpl };

    LineLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties = makeMutable<LineLayerProperties>(staticImmutableCast<LineLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers { layerProperties };

    // Check that `updateInternal()` is called even if the updated source has not yet loaded description.
    EXPECT_CALL(renderTilesetSource, mockedUpdateInternal()).Times(2);
    RenderSource* renderSource = &renderTilesetSource;
    renderSource->update(initialized.baseImpl, layers, true, true, test.tileParameters());

    VectorSource uninitialized("source", "http://url");
    renderSource->update(uninitialized.baseImpl, layers, true, true, test.tileParameters());
}

TEST(Source, GeoJSONSourceTilesAfterDataReset) {
    SourceTest test;
    GeoJSONSource source("source");
    auto geoJSONData = GeoJSONData::create(mapbox::geojson::parse(
        R"({"geometry": {"type": "Point", "coordinates": [1.1, 1.1]}, "type": "Feature", "properties": {}})"));
    source.setGeoJSONData(geoJSONData);
    RenderGeoJSONSource renderSource{staticImmutableCast<GeoJSONSource::Impl>(source.baseImpl)};

    CircleLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties =
        makeMutable<CircleLayerProperties>(staticImmutableCast<CircleLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers{layerProperties};

    static_cast<RenderSource&>(renderSource).update(source.baseImpl, layers, true, true, test.tileParameters());
    EXPECT_FALSE(renderSource.isLoaded()); // loaded == false, means that the source contains pending tiles.

    source.setGeoJSONData(nullptr);
    static_cast<RenderSource&>(renderSource).update(source.baseImpl, layers, true, true, test.tileParameters());
    EXPECT_FALSE(renderSource.isLoaded()); // Tiles remain in continous mode.

    source.setGeoJSONData(geoJSONData);
    static_cast<RenderSource&>(renderSource).update(source.baseImpl, layers, true, true, test.tileParameters());
    EXPECT_FALSE(renderSource.isLoaded());

    source.setGeoJSONData(nullptr);
    static_cast<RenderSource&>(renderSource)
        .update(source.baseImpl, layers, true, true, test.tileParameters(MapMode::Static));
    EXPECT_TRUE(renderSource.isLoaded()); // Tiles are reset in static mode.
}

TEST(Source, SetMaxParentOverscaleFactor) {
    SourceTest test;
    test.transform.jumpTo(CameraOptions().withCenter(LatLng()).withZoom(8.0));
    test.transformState = test.transform.getState();
    util::Timer timer;
    FixtureLog log;

    test.fileSource->tileResponse = [&](const Resource& res) {
        if (res.tileData->z == 5) {
            timer.start(Milliseconds(10), Duration::zero(), [&] { test.end(); });
        }
        // No tiles above zoom level 5 should be requested.
        EXPECT_LE(5, int(res.tileData->z));
        Response response;
        response.noContent = true;
        return response;
    };

    RasterLayer layer("id", "source");
    Immutable<LayerProperties> layerProperties =
        makeMutable<RasterLayerProperties>(staticImmutableCast<RasterLayer::Impl>(layer.baseImpl));
    std::vector<Immutable<LayerProperties>> layers{layerProperties};

    Tileset tileset;
    tileset.tiles = {"tiles"};

    RasterSource source("source", tileset, 512);
    ASSERT_EQ(nullopt, source.getMaxOverscaleFactorForParentTiles());
    source.setMaxOverscaleFactorForParentTiles(3);
    ASSERT_EQ(3, *source.getMaxOverscaleFactorForParentTiles());
    source.loadDescription(*test.fileSource);

    auto renderSource = RenderSource::create(source.baseImpl);
    renderSource->setObserver(&test.renderSourceObserver);
    renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());

    test.renderSourceObserver.tileChanged = [&](RenderSource&, const OverscaledTileID&) {
        renderSource->update(source.baseImpl, layers, true, true, test.tileParameters());
    };

    test.run();

    EXPECT_EQ(
        1u,
        log.count(
            {EventSeverity::Warning, Event::Style, -1, "Parent tile overscale factor will cap prefetch delta to 3"}));
    EXPECT_EQ(0u, log.uncheckedCount());
}
