#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/map_adapter.hpp>

#include <mbgl/map/map_options.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
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
        : frontend(1, gfx::HeadlessBackend::SwapBehaviour::NoFlush, gfx::ContextMode::Unique, fontFamily) {}

    util::RunLoop loop;
    std::shared_ptr<StubFileSource> fileSource = std::make_shared<StubFileSource>();
    HeadlessFrontend frontend;
    MapAdapter map { frontend, MapObserver::nullObserver(), fileSource,
                  MapOptions().withMapMode(MapMode::Static).withSize(frontend.getSize())};

    void checkRendering(const char * name, double imageMatchPixelsThreshold = 0.015, double pixelMatchThreshold = 0.1) {
        test::checkImage(std::string("test/fixtures/local_glyphs/") + name,
                         frontend.render(map).image,
                         imageMatchPixelsThreshold,
                         pixelMatchThreshold);
    }
};

} // end namespace

// Enabling Qt requires adding a CJK-compatible font in the CI image.
#if defined(__APPLE__)

TEST(LocalGlyphRasterizer, PingFang) {
    LocalGlyphRasterizerTest test(std::string("PingFang"));

    test.fileSource->glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/local_glyphs/mixed.json"));
#if defined(__APPLE__) && !defined(__QT__)
    test.checkRendering("ping_fang");
#elif defined(__QT__)
    test.checkRendering("ping_fang_qt");
#endif // defined(__APPLE__)
}

#endif // defined(__APPLE__)

#if defined(__linux__) && defined(__QT__)
TEST(LocalGlyphRasterizer, NotoSansCJK) {
    LocalGlyphRasterizerTest test(std::string("Noto Sans CJK KR Regular"));

    test.fileSource->glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/local_glyphs/mixed.json"));
    test.checkRendering("noto_sans_cjk_kr_regular_qt");
}
#endif // defined(__linux__) && defined(__QT__)

TEST(LocalGlyphRasterizer, NoLocal) {
    // Expectation: without any local fonts set, and without any CJK glyphs provided,
    // the output should just contain basic latin characters.
    LocalGlyphRasterizerTest test({});

    test.fileSource->glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/local_glyphs/mixed.json"));
    test.checkRendering("no_local", 0.001, 0.1);
}

TEST(LocalGlyphRasterizer, NoLocalWithContentInsets) {
    // Expectations: content insets imply rendering to offsetted viewport center.
    // Rendered text should be on the same offset and keep the same size as
    // with no offset.
    LocalGlyphRasterizerTest test({});

    test.fileSource->glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };
    auto viewSize = test.frontend.getSize();
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/local_glyphs/mixed.json"));

    // Expected image was created using center offset as in the line below - content insets,
    // with no pitch defined, should produce the same output.
    // test.map.moveBy({ viewSize.width * 0.25, 0 });

    test.map.jumpTo(CameraOptions().withPadding(EdgeInsets { 0, viewSize.width * 0.25 * 2, 0, 0 }));
    test.checkRendering("no_local_with_content_insets", 0.001, 0.1);
}

TEST(LocalGlyphRasterizer, NoLocalWithContentInsetsAndPitch) {
    // Expectations: content insets imply rendering to offsetted viewport center.
    // Rendered text should be on the same offset and keep the same size as
    // with no offset.
    LocalGlyphRasterizerTest test({});

    test.fileSource->glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };
    auto viewSize = test.frontend.getSize();
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/local_glyphs/mixed.json"));

    // Expected image was verified using no-padding render, offsetted to right
    // using bitmap editor.

    test.map.jumpTo(CameraOptions().withPitch(45).withPadding(EdgeInsets { 0, viewSize.width * 0.4, 0, 0 }));
    test.checkRendering("no_local_with_content_insets_and_pitch", 0.001, 0.1);
}
