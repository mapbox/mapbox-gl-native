#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/platform/factory.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/style/style.hpp>

/*
    LoadLocalCJKGlyph in glyph_manager.test.cpp exercises the platform-independent
    part of LocalGlyphRasterizer. This test actually exercises platform-dependent
    font loading code for whatever platform it runs on. Different platforms have
    different default fonts, so adding a new platform requires new "expected"
    fixtures.
 
    At the time of writing, we don't run `mbgl-test` on iOS or Android, so the only
    supported test platform is macOS. Supporting Android would require adding a new
    test case (probably using the "Droid" font family). iOS should theoretically
    work -- the "PingFang" font family used below is expected to be available on
    all iOS devices, and we use a relatively high image diff tolerance (0.05) to
    account for small changes between the many possible variants of the PingFang
    family.
*/

using namespace mbgl;

namespace {

class LocalGlyphRasterizerTest {
public:
    LocalGlyphRasterizerTest(const optional<std::string> fontFamily)
        : frontend(pixelRatio, stubFileSourceOptions, optional<std::string>(), GLContextMode::Unique, fontFamily)
    {
    }

    util::RunLoop loop;
    FileSourceOptions stubFileSourceOptions;
    std::shared_ptr<FileSource> fileSource = platform::Factory::sharedFileSource(
        stubFileSourceOptions, std::make_shared<StubFileSource>());
    float pixelRatio { 1 };
    HeadlessFrontend frontend { pixelRatio, stubFileSourceOptions };
    Map map { frontend, MapObserver::nullObserver(), frontend.getSize(), pixelRatio,
              MapOptions().withMapMode(MapMode::Static), stubFileSourceOptions };

    void checkRendering(const char * name) {
        test::checkImage(std::string("test/fixtures/local_glyphs/") + name,
                         frontend.render(map), 0.05, 0.1);
    }
};

} // end namespace

// Enabling Qt requires adding a CJK-compatible font in the CI image.
#if defined(__APPLE__)

TEST(LocalGlyphRasterizer, PingFang) {
    LocalGlyphRasterizerTest test(std::string("PingFang"));

    auto stubFileSource = std::static_pointer_cast<StubFileSource>(test.fileSource);
    stubFileSource->glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/local_glyphs/mixed.json"));
#if defined(__APPLE__)
    test.checkRendering("ping_fang");
#elif defined(__QT__)
    test.checkRendering("ping_fang_qt");
#endif // defined(__APPLE__)
}

#endif // defined(__APPLE__)

TEST(LocalGlyphRasterizer, NoLocal) {
    // Expectation: without any local fonts set, and without any CJK glyphs provided,
    // the output should just contain basic latin characters.
    LocalGlyphRasterizerTest test({});

    auto stubFileSource = std::static_pointer_cast<StubFileSource>(test.fileSource);
    stubFileSource->glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/local_glyphs/mixed.json"));
    test.checkRendering("no_local");
}

