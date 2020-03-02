#include <mbgl/map/camera.hpp>
#include <mbgl/map/map_snapshotter.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

using namespace mbgl;

class SnapshotterObserver final : public MapSnapshotterObserver {
public:
    ~SnapshotterObserver() = default;
    void onDidFinishLoadingStyle() override {
        if (didFinishLoadingStyleCallback) {
            didFinishLoadingStyleCallback();
        }
    }
    std::function<void()> didFinishLoadingStyleCallback;
};

TEST(MapSnapshotter, setStyleJSON) {
    util::RunLoop runLoop;
    MapSnapshotter snapshotter(Size{32, 16}, 1.0f, ResourceOptions());
    snapshotter.setStyleJSON(R"JSON({
        "version": 8,
        "layers": [{
            "id": "background",
            "type": "background",
            "paint": {"background-color": "green"}
        }]
    })JSON");

    snapshotter.snapshot([&runLoop](std::exception_ptr ptr,
                                    mbgl::PremultipliedImage image,
                                    mbgl::MapSnapshotter::Attributions,
                                    mbgl::MapSnapshotter::PointForFn,
                                    mbgl::MapSnapshotter::LatLngForFn) {
        EXPECT_EQ(nullptr, ptr);
        EXPECT_EQ(32, image.size.width);
        EXPECT_EQ(16, image.size.height);
        runLoop.stop();
    });

    runLoop.run();
}

TEST(MapSnapshotter, setSize) {
    util::RunLoop runLoop;
    MapSnapshotter snapshotter(Size{32, 16}, 1.0f, ResourceOptions());
    snapshotter.setStyleJSON(R"JSON({
        "version": 8,
        "layers": [{
            "id": "background",
            "type": "background",
            "paint": {"background-color": "green"}
        }]
    })JSON");

    snapshotter.setSize(Size{16, 32});

    snapshotter.snapshot([&runLoop](std::exception_ptr ptr,
                                    mbgl::PremultipliedImage image,
                                    mbgl::MapSnapshotter::Attributions,
                                    mbgl::MapSnapshotter::PointForFn,
                                    mbgl::MapSnapshotter::LatLngForFn) {
        EXPECT_EQ(nullptr, ptr);
        EXPECT_EQ(16, image.size.width);
        EXPECT_EQ(32, image.size.height);
        runLoop.stop();
    });

    runLoop.run();
}

TEST(MapSnapshotter, TEST_REQUIRES_SERVER(setStyleURL)) {
    util::RunLoop runLoop;
    MapSnapshotter snapshotter(Size{64, 32}, 1.0f, ResourceOptions());
    snapshotter.setStyleURL("http://127.0.0.1:3000/online/style.json");
    snapshotter.snapshot([&runLoop](std::exception_ptr ptr,
                                    mbgl::PremultipliedImage image,
                                    mbgl::MapSnapshotter::Attributions,
                                    mbgl::MapSnapshotter::PointForFn,
                                    mbgl::MapSnapshotter::LatLngForFn) {
        EXPECT_EQ(nullptr, ptr);
        EXPECT_EQ(64, image.size.width);
        EXPECT_EQ(32, image.size.height);
        runLoop.stop();
    });

    runLoop.run();
}

TEST(MapSnapshotter, TEST_REQUIRES_SERVER(cancel)) {
    util::RunLoop runLoop;
    MapSnapshotter snapshotter(Size{1, 1}, 1.0f, ResourceOptions());
    snapshotter.setStyleURL("http://127.0.0.1:3000/online/style.json");
    snapshotter.snapshot([](std::exception_ptr,
                            mbgl::PremultipliedImage,
                            mbgl::MapSnapshotter::Attributions,
                            mbgl::MapSnapshotter::PointForFn,
                            mbgl::MapSnapshotter::LatLngForFn) { ASSERT_TRUE(false); });

    snapshotter.cancel();

    util::Timer timer;
    timer.start(Milliseconds(100), Duration::zero(), [&runLoop] { runLoop.stop(); });

    runLoop.run();
}

TEST(MapSnapshotter, TEST_REQUIRES_SERVER(runtimeStyling)) {
    util::RunLoop runLoop;
    SnapshotterObserver observer;
    MapSnapshotter snapshotter(Size{256, 128}, 1.0f, ResourceOptions(), observer);
    snapshotter.setStyleURL("http://127.0.0.1:3000/online/style.json");

    observer.didFinishLoadingStyleCallback = [&snapshotter, &runLoop] {
        auto fillLayer = std::make_unique<style::FillLayer>("green_fill", "mapbox");
        fillLayer->setSourceLayer("water");
        fillLayer->setFillColor(Color(0.25, 0.88, 0.82, 1.0));
        snapshotter.getStyle().addLayer(std::move(fillLayer));
        snapshotter.snapshot([&runLoop](std::exception_ptr ptr,
                                        mbgl::PremultipliedImage image,
                                        mbgl::MapSnapshotter::Attributions,
                                        mbgl::MapSnapshotter::PointForFn,
                                        mbgl::MapSnapshotter::LatLngForFn) {
            EXPECT_EQ(nullptr, ptr);
            EXPECT_EQ(256, image.size.width);
            EXPECT_EQ(128, image.size.height);
            test::checkImage("test/fixtures/map/map_snapshotter_overlay", image, 0.005, 0.1);
            runLoop.stop();
        });
    };

    runLoop.run();
}
