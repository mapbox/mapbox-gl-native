#include "../fixtures/fixture_log_observer.hpp"
#include "../fixtures/util.hpp"
#include "mock_file_source.hpp"

#include <mbgl/map/sprite.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread.hpp>

using namespace mbgl;

using SpriteTestCallback = std::function<void(Sprite*, const Sprites&, std::exception_ptr)>;

struct SpriteParams {
    const std::string baseUrl;
    const float pixelRatio;
};

class SpriteThread : public Sprite::Observer {
public:
    SpriteThread(FileSource* fileSource, SpriteTestCallback callback) : callback_(callback) {
        util::ThreadContext::setFileSource(fileSource);
    }

    void loadSprite(const SpriteParams& params) {
        sprite_.reset(new Sprite(params.baseUrl, params.pixelRatio));
        sprite_->setObserver(this);
    }

    void unloadSprite() {
        sprite_->setObserver(nullptr);
        sprite_.reset();
    }

    void onSpriteLoaded(const Sprites& sprites) override {
        callback_(sprite_.get(), sprites, nullptr);
    }

    void onSpriteLoadingFailed(std::exception_ptr error) override {
        callback_(sprite_.get(), Sprites(), error);
    }

private:
    std::unique_ptr<Sprite> sprite_;
    SpriteTestCallback callback_;
};

class SpriteTest : public testing::Test {
protected:
    void runTest(const SpriteParams& params, FileSource* fileSource, SpriteTestCallback callback) {
        util::RunLoop loop(uv_default_loop());

        async_ = std::make_unique<uv::async>(loop.get(), [&] { loop.stop(); });
        async_->unref();

        const util::ThreadContext context = {"Map", util::ThreadType::Map, util::ThreadPriority::Regular};

        util::Thread<SpriteThread> tester(context, fileSource, callback);
        tester.invoke(&SpriteThread::loadSprite, params);

        uv_run(loop.get(), UV_RUN_DEFAULT);

        tester.invoke(&SpriteThread::unloadSprite);
    }

    void stopTest() {
        async_->send();
    }

private:
    std::unique_ptr<uv::async> async_;
};

TEST_F(SpriteTest, LoadingSuccess) {
    SpriteParams params = {
        "test/fixtures/resources/sprite",
        1.0,
    };

    auto callback = [this, &params](Sprite* sprite, const Sprites& sprites, std::exception_ptr error) {
        ASSERT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Map));

        ASSERT_TRUE(error == nullptr);

        ASSERT_TRUE(!sprites.empty());

        ASSERT_EQ(sprite->pixelRatio, params.pixelRatio);
        ASSERT_NE(sprite->pixelRatio, 1.5);
        ASSERT_NE(sprite->pixelRatio, 2.0);

        ASSERT_TRUE(sprite->isLoaded());

        stopTest();
    };

    MockFileSource fileSource(MockFileSource::Success, "");
    runTest(params, &fileSource, callback);
}

TEST_F(SpriteTest, LoadingFail) {
    SpriteParams params = {
        "test/fixtures/resources/sprite",
        1.0,
    };

    auto callback = [this, &params](Sprite* sprite, const Sprites&, std::exception_ptr error) {
        ASSERT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Map));

        ASSERT_TRUE(error != nullptr);

        ASSERT_EQ(sprite->pixelRatio, params.pixelRatio);
        ASSERT_NE(sprite->pixelRatio, 1.5);
        ASSERT_NE(sprite->pixelRatio, 2.0);

        ASSERT_FALSE(sprite->isLoaded());

        stopTest();
    };

    MockFileSource fileSourceFailSpriteJSON(MockFileSource::RequestFail, "sprite.json");
    runTest(params, &fileSourceFailSpriteJSON, callback);

    MockFileSource fileSourceFailSpriteImage(MockFileSource::RequestFail, "sprite.png");
    runTest(params, &fileSourceFailSpriteImage, callback);

    MockFileSource fileSourceCorruptedSpriteJSON(MockFileSource::RequestWithCorruptedData, "sprite.json");
    runTest(params, &fileSourceCorruptedSpriteJSON, callback);

    MockFileSource fileSourceCorruptedSpriteImage(MockFileSource::RequestWithCorruptedData, "sprite.png");
    runTest(params, &fileSourceCorruptedSpriteImage, callback);
}

TEST_F(SpriteTest, LoadingCancel) {
    SpriteParams params = {
        "test/fixtures/resources/sprite",
        1.0,
    };

    auto callback = [this](Sprite*, const Sprites&, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    MockFileSource fileSourceDelaySpriteJSON(MockFileSource::SuccessWithDelay, "sprite.json");
    fileSourceDelaySpriteJSON.setOnRequestDelayedCallback([this]{
        stopTest();
    });
    runTest(params, &fileSourceDelaySpriteJSON, callback);

    MockFileSource fileSourceDelaySpriteImage(MockFileSource::SuccessWithDelay, "sprite.png");
    fileSourceDelaySpriteImage.setOnRequestDelayedCallback([this]{
        stopTest();
    });
    runTest(params, &fileSourceDelaySpriteImage, callback);
}

TEST_F(SpriteTest, InvalidURL) {
    SpriteParams params = {
        "foo bar",
        1.0,
    };

    auto callback = [this](Sprite* sprite, const Sprites&, std::exception_ptr error) {
        ASSERT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Map));

        ASSERT_TRUE(error != nullptr);

        ASSERT_EQ(sprite->isLoaded(), false);

        stopTest();
    };

    MockFileSource fileSource(MockFileSource::Success, "");
    runTest(params, &fileSource, callback);
}
