#include "../fixtures/util.hpp"
#include "../fixtures/mock_file_source.hpp"
#include "../fixtures/mock_view.hpp"

#include <mbgl/style/style.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/util/thread.hpp>

namespace mbgl {

class MockStyleObserver: public Style::Observer {
public:
    using MockStyleObserverCallback = std::function<void(std::exception_ptr, Style*)>;

    MockStyleObserver(View& view_,
                      FileSource& fileSource_,
                      const MockStyleObserverCallback& callback_)
        : data(MapMode::Still, GLContextMode::Unique, view_.getPixelRatio()),
          style(std::make_unique<Style>(data)),
          transform(view_, ConstrainMode::HeightOnly),
          callback(callback_) {
        util::ThreadContext::setFileSource(&fileSource_);

        transform.resize({{ 1000, 1000 }});
        transform.setLatLngZoom({0, 0}, 16);

        const std::string style_ = util::read_file("test/fixtures/resources/style-unused-sources.json");
        style->setJSON(style_, "");
        style->setObserver(this);
    }

    ~MockStyleObserver() {
        cleanup();
    }

    void cleanup() {
        style.reset();
    }

    void update() {
        const auto now = Clock::now();

        data.setAnimationTime(now);
        transform.updateTransitions(now);

        style->cascade();
        style->recalculate(16);
        style->update(transform.getState(), texturePool);
    }

    // Style::Observer implementation.
    void onTileDataChanged() override {
        update();

        if (style->isLoaded()) {
            callback(nullptr, style.get());
        }
    }

    void onResourceLoadingFailed(std::exception_ptr error) override {
        callback(error, style.get());
    }

private:
    MapData data;
    std::unique_ptr<Style> style;
    Transform transform;
    TexturePool texturePool;

    MockStyleObserverCallback callback;
};

TEST(Style, UnusedSources) {
    util::RunLoop loop;

    MockView view;
    MockFileSource fileSource(MockFileSource::Success, "");

    auto callback = [&loop](std::exception_ptr error, Style* style) {
        EXPECT_TRUE(error == nullptr);
        loop.stop();

        Source *usedSource = style->getSource("usedsource");
        EXPECT_TRUE(usedSource);
        EXPECT_TRUE(usedSource->isLoaded());

        Source *unusedSource = style->getSource("unusedsource");
        EXPECT_TRUE(unusedSource);
        EXPECT_FALSE(unusedSource->isLoaded());
    };

    std::unique_ptr<util::Thread<MockStyleObserver>> observer(
        std::make_unique<util::Thread<MockStyleObserver>>(
            util::ThreadContext{"Map", util::ThreadType::Map, util::ThreadPriority::Regular}, view, fileSource, callback));

    loop.run();

    // Needed because it will make the Map thread
    // join and cease logging after this point.
    observer->invoke(&MockStyleObserver::cleanup);
    observer.reset();
}

} // namespace mbgl
