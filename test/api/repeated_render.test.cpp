#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/test/stub_renderer_frontend.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>

#include <future>

using namespace mbgl;


TEST(API, RepeatedRender) {

    util::RunLoop loop;

    const auto style = util::read_file("test/fixtures/api/water.json");

    HeadlessBackend backend;
    BackendScope scope { backend };
    OffscreenView view { backend.getContext(), { 512, 512 } };
    float pixelRatio { 1 };
    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets");
    ThreadPool threadPool(4);
    StubRendererFrontend rendererFrontend {
            std::make_unique<Renderer>(backend, pixelRatio, fileSource, threadPool), view };

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    Map map(rendererFrontend, MapObserver::nullObserver(), view.getSize(), pixelRatio, fileSource,
            threadPool, MapMode::Still);

    {
        map.getStyle().loadJSON(style);
        PremultipliedImage result;
        map.renderStill([&](std::exception_ptr) {
            result = view.readStillImage();
        });

        while (!result.valid()) {
            loop.runOnce();
        }

        ASSERT_EQ(512u, result.size.width);
        ASSERT_EQ(512u, result.size.height);
        test::checkImage("test/fixtures/api/repeated_render", result, 0.0003, 0.1);
    }

    {
        map.getStyle().loadJSON(style);
        PremultipliedImage result;
        map.renderStill([&](std::exception_ptr) {
            result = view.readStillImage();
        });

        while (!result.valid()) {
            loop.runOnce();
        }

        ASSERT_EQ(512u, result.size.width);
        ASSERT_EQ(512u, result.size.height);
        test::checkImage("test/fixtures/api/repeated_render", result, 0.0003, 0.1);
    }

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    auto unchecked = flo->unchecked();
    EXPECT_TRUE(unchecked.empty()) << unchecked;
}
