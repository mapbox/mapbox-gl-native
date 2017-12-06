#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/style/style.hpp>

using namespace mbgl;

namespace {

class LocalGlyphRasterizerTest {
public:
    LocalGlyphRasterizerTest(const optional<std::string> fontFamily)
        : frontend(pixelRatio, fileSource, threadPool, optional<std::string>(), GLContextMode::Unique, fontFamily)
    {
    }

    util::RunLoop loop;
    StubFileSource fileSource;
    ThreadPool threadPool { 4 };
    float pixelRatio { 1 };
    HeadlessFrontend frontend;
    Map map { frontend, MapObserver::nullObserver(), frontend.getSize(), pixelRatio, fileSource,
              threadPool, MapMode::Static};

    void checkRendering(const char * name) {
        test::checkImage(std::string("test/fixtures/local_glyphs/") + name,
                         frontend.render(map), 0.05, 0.1);
    }
};

} // end namespace

#ifdef __APPLE__

TEST(LocalGlyphRasterizer, PingFang) {
    LocalGlyphRasterizerTest test(std::string("PingFang"));

    test.fileSource.glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/local_glyphs/mixed.json"));
    test.checkRendering("ping_fang");
}

#endif

TEST(LocalGlyphRasterizer, NoLocal) {
    // Expectation: without any local fonts set, and without any CJK glyphs provided,
    // the output should just contain basic latin characters.
    LocalGlyphRasterizerTest test({});

    test.fileSource.glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/local_glyphs/mixed.json"));
    test.checkRendering("no_local");
}

#if ANDROID

TEST(LocalGlyphRasterizer, Droid) {
    LocalGlyphRasterizerTest test(std::string("Droid"));

    test.fileSource.glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/local_glyphs/mixed.json"));
    test.checkRendering("droid");
}

#endif
