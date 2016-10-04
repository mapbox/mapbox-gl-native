#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;

namespace {

class QueryTest {
public:
    QueryTest() {
        auto decoded = decodeImage(util::read_file("test/fixtures/sprites/default_marker.png"));
        auto image = std::make_unique<SpriteImage>(std::move(decoded), 1.0);

        map.setStyleJSON(util::read_file("test/fixtures/api/query_style.json"));
        map.addImage("test-icon", std::move(image));

        test::render(map);
    }

    util::RunLoop loop;
    std::shared_ptr<HeadlessDisplay> display { std::make_shared<HeadlessDisplay>() };
    HeadlessView view { display, 1 };
    StubFileSource fileSource;
    Map map { view, fileSource, MapMode::Still };
};

} // end namespace

TEST(Query, QueryRenderedFeatures) {
    QueryTest test;

    auto features1 = test.map.queryRenderedFeatures(test.map.pixelForLatLng({ 0, 0 }));
    EXPECT_EQ(features1.size(), 3u);

    auto features2 = test.map.queryRenderedFeatures(test.map.pixelForLatLng({ 9, 9 }));
    EXPECT_EQ(features2.size(), 0u);
}

TEST(Query, QueryRenderedFeaturesFilterLayer) {
    QueryTest test;

    auto zz = test.map.pixelForLatLng({ 0, 0 });

    auto features1 = test.map.queryRenderedFeatures(zz, {{ "layer1"}});
    EXPECT_EQ(features1.size(), 1u);

    auto features2 = test.map.queryRenderedFeatures(zz, {{ "layer1", "layer2" }});
    EXPECT_EQ(features2.size(), 2u);

    auto features3 = test.map.queryRenderedFeatures(zz, {{ "foobar" }});
    EXPECT_EQ(features3.size(), 0u);

    auto features4 = test.map.queryRenderedFeatures(zz, {{ "foobar", "layer3" }});
    EXPECT_EQ(features4.size(), 1u);
}
