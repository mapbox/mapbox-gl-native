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
        tileDataChanged();
    }

    void onResourceLoadingFailed(std::exception_ptr error) override {
        resourceLoadingFailed(error);
    }

    std::function<void ()> tileDataChanged = [] {};
    std::function<void (std::exception_ptr)> resourceLoadingFailed = [] (auto) {};
};

std::string resourceErrorString(MockFileSource::Type type, const std::string& param) {
    Log::setObserver(std::make_unique<Log::NullObserver>()); // Squelch logging.

    util::RunLoop loop;

    util::ThreadContext context("Map", util::ThreadType::Map, util::ThreadPriority::Regular);
    util::ThreadContext::Set(&context);

    MockFileSource fileSource(type, param);
    util::ThreadContext::setFileSource(&fileSource);

    MapData data(MapMode::Still, GLContextMode::Unique, 1.0);
    MockView view;
    Transform transform(view, ConstrainMode::HeightOnly);
    TexturePool texturePool;
    Style style(data);

    StyleObserver observer;
    std::string result;

    observer.tileDataChanged = [&] () {
        // Prompt tile loading after sources load.
        style.update(transform.getState(), texturePool);

        if (style.isLoaded()) {
            loop.stop();
        }
    };

    observer.resourceLoadingFailed = [&] (std::exception_ptr error) {
        result = util::toString(error);
        loop.stop();
    };

    transform.resize({{ 512, 512 }});
    transform.setLatLngZoom({0, 0}, 0);

    style.setObserver(&observer);
    style.setJSON(util::read_file("test/fixtures/resources/style.json"), "");
    style.cascade();
    style.recalculate(16);

    loop.run();

    return result;
}

TEST(ResourceLoading, Success) {
    EXPECT_EQ(resourceErrorString(MockFileSource::Success, ""), "");
}

TEST(ResourceLoading, RasterSourceFail) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestFail, "source_raster.json"),
        "Failed to load [test/fixtures/resources/source_raster.json]: Failed by the test case");
}

TEST(ResourceLoading, VectorSourceFail) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestFail, "source_vector.json"),
        "Failed to load [test/fixtures/resources/source_vector.json]: Failed by the test case");
}

TEST(ResourceLoading, SpriteJSONFail) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestFail, "sprite.json"),
        "Failed to load [test/fixtures/resources/sprite.json]: Failed by the test case");
}

TEST(ResourceLoading, SpriteImageFail) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestFail, "sprite.png"),
        "Failed to load [test/fixtures/resources/sprite.png]: Failed by the test case");
}

TEST(ResourceLoading, RasterTileFail) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestFail, "raster.png"),
        "Failed to load [test/fixtures/resources/raster.png]: Failed by the test case");
}

TEST(ResourceLoading, VectorTileFail) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestFail, "vector.pbf"),
        "Failed to load [test/fixtures/resources/vector.pbf]: Failed by the test case");
}

TEST(ResourceLoading, GlyphsFail) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestFail, "glyphs.pbf"),
        "Failed to load [test/fixtures/resources/glyphs.pbf]: Failed by the test case");
}

TEST(ResourceLoading, RasterSourceCorrupt) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestWithCorruptedData, "source_raster.json"),
        "Failed to parse [test/fixtures/resources/source_raster.json]: 0 - Invalid value.");
}

TEST(ResourceLoading, VectorSourceCorrupt) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestWithCorruptedData, "source_vector.json"),
        "Failed to parse [test/fixtures/resources/source_vector.json]: 0 - Invalid value.");
}

TEST(ResourceLoading, SpriteJSONCorrupt) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestWithCorruptedData, "sprite.json"),
        "Failed to parse JSON: Invalid value. at offset 0");
}

TEST(ResourceLoading, SpriteImageCorrupt) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestWithCorruptedData, "sprite.png"),
        "Could not parse sprite image");
}

TEST(ResourceLoading, RasterTileCorrupt) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestWithCorruptedData, "raster.png"),
        "Failed to parse [0/0/0]: error parsing raster image");
}

TEST(ResourceLoading, VectorTileCorrupt) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestWithCorruptedData, "vector.pbf"),
        "Failed to parse [0/0/0]: pbf unknown field type exception");
}

TEST(ResourceLoading, GlyphsCorrupt) {
    EXPECT_EQ(resourceErrorString(MockFileSource::RequestWithCorruptedData, "glyphs.pbf"),
        "Failed to parse [test/fixtures/resources/glyphs.pbf]: pbf unknown field type exception");
}
