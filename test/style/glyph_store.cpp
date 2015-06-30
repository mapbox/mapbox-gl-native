#include "../fixtures/fixture_log_observer.hpp"
#include "../fixtures/mock_file_source.hpp"
#include "../fixtures/util.hpp"

#include <mbgl/text/font_stack.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread.hpp>

using namespace mbgl;

using GlyphStoreTestCallback = std::function<void(GlyphStore*, std::exception_ptr)>;

struct GlyphStoreParams {
    const std::string url;
    const std::string stack;
    const std::set<GlyphRange> ranges;
};

class GlyphStoreThread : public GlyphStore::Observer {
public:
    GlyphStoreThread(FileSource* fileSource, GlyphStoreTestCallback callback) : callback_(callback) {
        util::ThreadContext::setFileSource(fileSource);
    }

    void loadGlyphStore(const GlyphStoreParams& params) {
        glyphStore_.reset(new GlyphStore());

        glyphStore_->setObserver(this);
        glyphStore_->setURL(params.url);

        ASSERT_FALSE(glyphStore_->hasGlyphRanges(params.stack, params.ranges));
    }

    void unloadGlyphStore() {
        glyphStore_->setObserver(nullptr);
        glyphStore_.reset();
    }

    void onGlyphRangeLoaded() override {
        callback_(glyphStore_.get(), nullptr);
    }

    void onGlyphRangeLoadingFailed(std::exception_ptr error) override {
        callback_(glyphStore_.get(), error);
    }

private:
    std::unique_ptr<GlyphStore> glyphStore_;
    GlyphStoreTestCallback callback_;
};

class GlyphStoreTest : public testing::Test {
protected:
    void runTest(const GlyphStoreParams& params, FileSource* fileSource, GlyphStoreTestCallback callback) {
        util::RunLoop loop(uv_default_loop());

        async_ = std::make_unique<uv::async>(loop.get(), [&]{ loop.stop(); });
        async_->unref();

        const util::ThreadContext context = {"Map", util::ThreadType::Map, util::ThreadPriority::Regular};

        util::Thread<GlyphStoreThread> tester(context, fileSource, callback);
        tester.invoke(&GlyphStoreThread::loadGlyphStore, params);

        uv_run(loop.get(), UV_RUN_DEFAULT);

        tester.invoke(&GlyphStoreThread::unloadGlyphStore);
    }

    void stopTest() {
        testDone = true;
        async_->send();
    }

    bool isDone() const {
        return testDone;
    }

private:
    bool testDone = false;

    std::unique_ptr<uv::async> async_;
};

TEST_F(GlyphStoreTest, LoadingSuccess) {
    GlyphStoreParams params = {
        "test/fixtures/resources/glyphs.pbf",
        "Test Stack",
        {{0, 255}, {256, 511}}
    };

    auto callback = [this, &params](GlyphStore* store, std::exception_ptr error) {
        ASSERT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Map));

        // We need to check if the test is over because checking
        // if the GlyphStore has glyphs below will cause more requests
        // to happen and we don't want this endless loop.
        if (isDone()) {
            return;
        }

        ASSERT_EQ(error, nullptr);

        if (!store->hasGlyphRanges(params.stack, params.ranges)) {
            return;
        }

        ASSERT_FALSE(store->hasGlyphRanges("Foobar", params.ranges));
        ASSERT_FALSE(store->hasGlyphRanges("Foobar", {{512, 767}}));
        ASSERT_FALSE(store->hasGlyphRanges("Test Stack",  {{512, 767}}));

        auto fontStack = store->getFontStack(params.stack);
        ASSERT_FALSE(fontStack->getMetrics().empty());
        ASSERT_FALSE(fontStack->getSDFs().empty());

        stopTest();
    };

    MockFileSource fileSource(MockFileSource::Success, "");
    runTest(params, &fileSource, callback);
}

TEST_F(GlyphStoreTest, LoadingFail) {
    GlyphStoreParams params = {
        "test/fixtures/resources/glyphs.pbf",
        "Test Stack",
        {{0, 255}, {256, 511}}
    };

    auto callback = [this, &params](GlyphStore* store, std::exception_ptr error) {
        ASSERT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Map));

        if (isDone()) {
            return;
        }

        ASSERT_TRUE(error != nullptr);

        auto fontStack = store->getFontStack(params.stack);
        ASSERT_EQ(*fontStack, nullptr);

        for (const auto& range : params.ranges) {
            ASSERT_FALSE(store->hasGlyphRanges(params.stack, {range}));
        }

        ASSERT_FALSE(store->hasGlyphRanges(params.stack, params.ranges));
        ASSERT_FALSE(store->hasGlyphRanges("Foobar", params.ranges));
        ASSERT_FALSE(store->hasGlyphRanges("Foobar", {{512, 767}}));

        stopTest();
    };

    MockFileSource fileSource(MockFileSource::RequestFail, "glyphs.pbf");
    runTest(params, &fileSource, callback);
}

TEST_F(GlyphStoreTest, LoadingCorrupted) {
    GlyphStoreParams params = {
        "test/fixtures/resources/glyphs.pbf",
        "Test Stack",
        {{0, 255}, {256, 511}}
    };

    auto callback = [this, &params](GlyphStore* store, std::exception_ptr error) {
        ASSERT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Map));

        if (isDone()) {
            return;
        }

        ASSERT_TRUE(error != nullptr);

        for (const auto& range : params.ranges) {
            ASSERT_FALSE(store->hasGlyphRanges(params.stack, {range}));
        }

        ASSERT_FALSE(store->hasGlyphRanges(params.stack, params.ranges));
        ASSERT_FALSE(store->hasGlyphRanges("Foobar", params.ranges));
        ASSERT_FALSE(store->hasGlyphRanges("Foobar", {{512, 767}}));

        stopTest();
    };

    MockFileSource fileSource(MockFileSource::RequestWithCorruptedData, "glyphs.pbf");
    runTest(params, &fileSource, callback);
}

TEST_F(GlyphStoreTest, LoadingCancel) {
    GlyphStoreParams params = {
        "test/fixtures/resources/glyphs.pbf",
        "Test Stack",
        {{0, 255}, {256, 511}}
    };

    auto callback = [this](GlyphStore*, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    MockFileSource fileSource(MockFileSource::SuccessWithDelay, "glyphs.pbf");
    fileSource.setOnRequestDelayedCallback([this]{
        stopTest();
    });
    runTest(params, &fileSource, callback);
}

TEST_F(GlyphStoreTest, InvalidURL) {
    GlyphStoreParams params = {
        "foo bar",
        "Test Stack",
        {{0, 255}, {256, 511}}
    };

    auto callback = [this, &params](GlyphStore* store, std::exception_ptr error) {
        ASSERT_TRUE(error != nullptr);

        auto fontStack = store->getFontStack(params.stack);
        ASSERT_EQ(*fontStack, nullptr);

        stopTest();
    };

    MockFileSource fileSource(MockFileSource::Success, "");
    runTest(params, &fileSource, callback);
}
