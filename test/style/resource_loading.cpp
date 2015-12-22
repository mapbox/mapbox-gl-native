#include "../fixtures/fixture_log_observer.hpp"
#include "../fixtures/util.hpp"
#include "../fixtures/mock_file_source.hpp"
#include "../fixtures/mock_view.hpp"

#include <mbgl/map/map_data.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/string.hpp>

using namespace mbgl;

class StyleObserver : public Style::Observer {
public:
    void onGlyphsLoaded(const std::string& fontStack, const GlyphRange& glyphRange) override {
        if (glyphsLoaded) glyphsLoaded(fontStack, glyphRange);
    }

    void onGlyphsError(const std::string& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) override {
        if (glyphsError) glyphsError(fontStack, glyphRange, error);
    }

    void onSpriteLoaded() override {
        if (spriteLoaded) spriteLoaded();
    }

    void onSpriteError(std::exception_ptr error) override {
        if (spriteError) spriteError(error);
    }

    void onSourceLoaded(Source& source) override {
        if (sourceLoaded) sourceLoaded(source);
    }

    void onSourceError(Source& source, std::exception_ptr error) override {
        if (sourceError) sourceError(source, error);
    }

    void onTileLoaded(Source& source, const TileID& tileID, bool isNewTile) override {
        if (tileLoaded) tileLoaded(source, tileID, isNewTile);
    }

    void onTileError(Source& source, const TileID& tileID, std::exception_ptr error) override {
        if (tileError) tileError(source, tileID, error);
    }

    void onResourceLoaded() override {
        if (resourceLoaded) resourceLoaded();
    };

    void onResourceError(std::exception_ptr error) override {
        if (resourceError) resourceError(error);
    };

    std::function<void (const std::string& fontStack, const GlyphRange&)> glyphsLoaded;
    std::function<void (const std::string& fontStack, const GlyphRange&, std::exception_ptr)> glyphsError;
    std::function<void ()> spriteLoaded;
    std::function<void (std::exception_ptr)> spriteError;
    std::function<void (Source&)> sourceLoaded;
    std::function<void (Source&, std::exception_ptr)> sourceError;
    std::function<void (Source&, const TileID&, bool isNewTile)> tileLoaded;
    std::function<void (Source&, const TileID&, std::exception_ptr)> tileError;
    std::function<void ()> resourceLoaded;
    std::function<void (std::exception_ptr)> resourceError;

    std::function<void ()> fullyLoaded;
};

class ResourceLoadingTest {
public:
    ResourceLoadingTest(MockFileSource::Type type, const std::string& resource)
        : fileSource(type, resource) {}

    util::ThreadContext context { "Map", util::ThreadType::Map, util::ThreadPriority::Regular };

    MockFileSource fileSource;
    StyleObserver observer;
    util::RunLoop loop;

    MapData data { MapMode::Still, GLContextMode::Unique, 1.0 };
    MockView view;
    Transform transform { view, ConstrainMode::HeightOnly };
    TexturePool texturePool;
    Style style { data };

    void run(const std::string& stylePath) {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        util::ThreadContext::Set(&context);
        util::ThreadContext::setFileSource(&fileSource);

        observer.resourceLoaded = [&] () {
            style.update(transform.getState(), texturePool);
            if (style.isLoaded() && observer.fullyLoaded) {
                observer.fullyLoaded();
            }
        };

        transform.resize({{ 512, 512 }});
        transform.setLatLngZoom({0, 0}, 0);

        style.setObserver(&observer);
        style.setJSON(util::read_file(stylePath), "");
        style.cascade();
        style.recalculate(0);

        loop.run();
    }

    void end() {
        loop.stop();
    }
};

TEST(ResourceLoading, Success) {
    ResourceLoadingTest test(MockFileSource::Success, "");

    test.observer.resourceError = [&] (std::exception_ptr error) {
        FAIL() << util::toString(error);
    };

    test.observer.fullyLoaded = [&] () {
        SUCCEED();
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, RasterSourceFail) {
    ResourceLoadingTest test(MockFileSource::RequestFail, "source_raster.json");

    test.observer.sourceError = [&] (Source& source, std::exception_ptr error) {
        EXPECT_EQ(source.info.source_id, "rastersource");
        EXPECT_EQ(util::toString(error), "Failed by the test case");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, VectorSourceFail) {
    ResourceLoadingTest test(MockFileSource::RequestFail, "source_vector.json");

    test.observer.sourceError = [&] (Source& source, std::exception_ptr error) {
        EXPECT_EQ(source.info.source_id, "vectorsource");
        EXPECT_EQ(util::toString(error), "Failed by the test case");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, SpriteJSONFail) {
    ResourceLoadingTest test(MockFileSource::RequestFail, "sprite.json");

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_EQ(util::toString(error), "Failed by the test case");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, SpriteImageFail) {
    ResourceLoadingTest test(MockFileSource::RequestFail, "sprite.png");

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_EQ(util::toString(error), "Failed by the test case");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, RasterTileFail) {
    ResourceLoadingTest test(MockFileSource::RequestFail, "raster.png");

    test.observer.tileError = [&] (Source& source, const TileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source.info.source_id, "rastersource");
        EXPECT_EQ(std::string(tileID), "0/0/0");
        EXPECT_EQ(util::toString(error), "Failed by the test case");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, VectorTileFail) {
    ResourceLoadingTest test(MockFileSource::RequestFail, "vector.pbf");

    test.observer.tileError = [&] (Source& source, const TileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source.info.source_id, "vectorsource");
        EXPECT_EQ(std::string(tileID), "0/0/0");
        EXPECT_EQ(util::toString(error), "Failed by the test case");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, GlyphsFail) {
    ResourceLoadingTest test(MockFileSource::RequestFail, "glyphs.pbf");

    test.observer.glyphsError = [&] (const std::string& fontStack, const GlyphRange&, std::exception_ptr error) {
        EXPECT_EQ(fontStack, "Open Sans Regular,Arial Unicode MS Regular");
        EXPECT_EQ(util::toString(error), "Failed by the test case");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, RasterSourceCorrupt) {
    ResourceLoadingTest test(MockFileSource::RequestWithCorruptedData, "source_raster.json");

    test.observer.sourceError = [&] (Source& source, std::exception_ptr error) {
        EXPECT_EQ(source.info.source_id, "rastersource");
        EXPECT_EQ(util::toString(error), "0 - Invalid value.");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, VectorSourceCorrupt) {
    ResourceLoadingTest test(MockFileSource::RequestWithCorruptedData, "source_vector.json");

    test.observer.sourceError = [&] (Source& source, std::exception_ptr error) {
        EXPECT_EQ(source.info.source_id, "vectorsource");
        EXPECT_EQ(util::toString(error), "0 - Invalid value.");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, SpriteJSONCorrupt) {
    ResourceLoadingTest test(MockFileSource::RequestWithCorruptedData, "sprite.json");

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_EQ(util::toString(error), "Failed to parse JSON: Invalid value. at offset 0");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, SpriteImageCorrupt) {
    ResourceLoadingTest test(MockFileSource::RequestWithCorruptedData, "sprite.png");

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(bool(error));
        // Not asserting on platform-specific error text.
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, RasterTileCorrupt) {
    ResourceLoadingTest test(MockFileSource::RequestWithCorruptedData, "raster.png");

    test.observer.tileError = [&] (Source& source, const TileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source.info.source_id, "rastersource");
        EXPECT_EQ(std::string(tileID), "0/0/0");
        EXPECT_TRUE(bool(error));
        // Not asserting on platform-specific error text.
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, VectorTileCorrupt) {
    ResourceLoadingTest test(MockFileSource::RequestWithCorruptedData, "vector.pbf");

    test.observer.tileError = [&] (Source& source, const TileID& tileID, std::exception_ptr error) {
        EXPECT_EQ(source.info.source_id, "vectorsource");
        EXPECT_EQ(std::string(tileID), "0/0/0");
        EXPECT_EQ(util::toString(error), "pbf unknown field type exception");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, GlyphsCorrupt) {
    ResourceLoadingTest test(MockFileSource::RequestWithCorruptedData, "glyphs.pbf");

    test.observer.glyphsError = [&] (const std::string& fontStack, const GlyphRange&, std::exception_ptr error) {
        EXPECT_EQ(fontStack, "Open Sans Regular,Arial Unicode MS Regular");
        EXPECT_EQ(util::toString(error), "pbf unknown field type exception");
        test.end();
    };

    test.run("test/fixtures/resources/style.json");
}

TEST(ResourceLoading, UnusedSource) {
    ResourceLoadingTest test(MockFileSource::Success, "");

    test.observer.fullyLoaded = [&] () {
        Source *usedSource = test.style.getSource("usedsource");
        EXPECT_TRUE(usedSource);
        EXPECT_TRUE(usedSource->isLoaded());

        Source *unusedSource = test.style.getSource("unusedsource");
        EXPECT_TRUE(unusedSource);
        EXPECT_FALSE(unusedSource->isLoaded());

        test.end();
    };

    test.run("test/fixtures/resources/style-unused-sources.json");
}

TEST(ResourceLoading, UnusedSourceActiveViaClassUpdate) {
    ResourceLoadingTest test(MockFileSource::Success, "");

    test.data.addClass("visible");

    test.observer.fullyLoaded = [&] () {
        Source *unusedSource = test.style.getSource("unusedsource");
        EXPECT_TRUE(unusedSource);
        EXPECT_TRUE(unusedSource->isLoaded());

        test.end();
    };

    test.run("test/fixtures/resources/style-unused-sources.json");
}
