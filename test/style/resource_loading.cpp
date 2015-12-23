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
    void onTileDataChanged() override {
        if (tileDataChanged) tileDataChanged();
    }

    void onResourceLoadingFailed(std::exception_ptr error) override {
        if (resourceLoadingFailed) resourceLoadingFailed(error);
    }

    std::function<void ()> tileDataChanged = nullptr;
    std::function<void (std::exception_ptr)> resourceLoadingFailed = nullptr;
};

struct StyleTestData {
    MockFileSource::Type type;
    std::string style;
    std::string resource;
    std::string expectedError;
    std::string styleClass;
    std::function<void (const Style&)> callback = nullptr;
};

void runTest(StyleTestData& testData) {
    Log::setObserver(std::make_unique<Log::NullObserver>()); // Squelch logging.

    util::RunLoop loop;

    util::ThreadContext context("Map", util::ThreadType::Map, util::ThreadPriority::Regular);
    util::ThreadContext::Set(&context);

    MockFileSource fileSource(testData.type, testData.resource);
    util::ThreadContext::setFileSource(&fileSource);

    MapData data(MapMode::Still, GLContextMode::Unique, 1.0);
    MockView view;
    Transform transform(view, ConstrainMode::HeightOnly);
    TexturePool texturePool;
    Style style(data);

    StyleObserver observer;

    observer.tileDataChanged = [&] () {
        // Prompt tile loading after sources load.
        style.update(transform.getState(), texturePool);

        if (style.isLoaded()) {
            loop.stop();
            if (testData.callback) testData.callback(style);
        }
    };

    observer.resourceLoadingFailed = [&] (std::exception_ptr error) {
        loop.stop();
        testData.expectedError = util::toString(error);
    };

    transform.resize({{ 512, 512 }});
    transform.setLatLngZoom({0, 0}, 0);

    style.setObserver(&observer);
    style.setJSON(util::read_file(testData.style), "");

    if (!testData.styleClass.empty()) data.addClass(testData.styleClass);

    style.cascade();
    style.recalculate(16);

    loop.run();
}

TEST(ResourceLoading, Success) {
    StyleTestData testData;
    testData.type = MockFileSource::Success;
    testData.style = "test/fixtures/resources/style.json";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "");
}

TEST(ResourceLoading, RasterSourceFail) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestFail;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "source_raster.json";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to load [test/fixtures/resources/source_raster.json]: Failed by the test case");
}

TEST(ResourceLoading, VectorSourceFail) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestFail;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "source_vector.json";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to load [test/fixtures/resources/source_vector.json]: Failed by the test case");
}

TEST(ResourceLoading, SpriteJSONFail) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestFail;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "sprite.json";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to load [test/fixtures/resources/sprite.json]: Failed by the test case");
}

TEST(ResourceLoading, SpriteImageFail) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestFail;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "sprite.png";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to load [test/fixtures/resources/sprite.png]: Failed by the test case");
}

TEST(ResourceLoading, RasterTileFail) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestFail;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "raster.png";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to load [test/fixtures/resources/raster.png]: Failed by the test case");
}

TEST(ResourceLoading, VectorTileFail) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestFail;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "vector.pbf";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to load [test/fixtures/resources/vector.pbf]: Failed by the test case");
}

TEST(ResourceLoading, GlyphsFail) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestFail;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "glyphs.pbf";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to load [test/fixtures/resources/glyphs.pbf]: Failed by the test case");
}

TEST(ResourceLoading, RasterSourceCorrupt) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestWithCorruptedData;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "source_raster.json";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to parse [test/fixtures/resources/source_raster.json]: 0 - Invalid value.");
}

TEST(ResourceLoading, VectorSourceCorrupt) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestWithCorruptedData;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "source_vector.json";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to parse [test/fixtures/resources/source_vector.json]: 0 - Invalid value.");
}

TEST(ResourceLoading, SpriteJSONCorrupt) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestWithCorruptedData;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "sprite.json";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to parse JSON: Invalid value. at offset 0");
}

TEST(ResourceLoading, SpriteImageCorrupt) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestWithCorruptedData;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "sprite.png";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Could not parse sprite image");
}

TEST(ResourceLoading, RasterTileCorrupt) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestWithCorruptedData;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "raster.png";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to parse [0/0/0]: error parsing raster image");
}

TEST(ResourceLoading, VectorTileCorrupt) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestWithCorruptedData;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "vector.pbf";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to parse [0/0/0]: pbf unknown field type exception");
}

TEST(ResourceLoading, GlyphsCorrupt) {
    StyleTestData testData;
    testData.type = MockFileSource::RequestWithCorruptedData;
    testData.style = "test/fixtures/resources/style.json";
    testData.resource = "glyphs.pbf";
    runTest(testData);
    EXPECT_EQ(testData.expectedError, "Failed to parse [test/fixtures/resources/glyphs.pbf]: pbf unknown field type exception");
}

TEST(ResourceLoading, UnusedSource) {
    StyleTestData testData;
    testData.type = MockFileSource::Success;
    testData.style = "test/fixtures/resources/style-unused-sources.json";
    testData.callback = [] (const Style& style) {
        Source *usedSource = style.getSource("usedsource");
        EXPECT_TRUE(usedSource);
        EXPECT_TRUE(usedSource->isLoaded());

        Source *unusedSource = style.getSource("unusedsource");
        EXPECT_TRUE(unusedSource);
        EXPECT_FALSE(unusedSource->isLoaded());
    };
    runTest(testData);
}

TEST(ResourceLoading, UnusedSourceActiveViaClassUpdate) {
    StyleTestData testData;
    testData.type = MockFileSource::Success;
    testData.style = "test/fixtures/resources/style-unused-sources.json";
    testData.styleClass = "visible";
    testData.callback = [] (const Style& style) {
        Source *unusedSource = style.getSource("unusedsource");
        EXPECT_TRUE(unusedSource);
        EXPECT_TRUE(unusedSource->isLoaded());
    };
    runTest(testData);
}
