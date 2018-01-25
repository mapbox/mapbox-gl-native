#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/actor/actor_ref.hpp>

#include <gtest/gtest.h>
#include <atomic>

using namespace mbgl;

#if !ANDROID
TEST(AssetFileSource, Load) {
    util::RunLoop loop;

    AssetFileSource fs("test/fixtures/storage/assets");

    // iOS seems to run out of file descriptors...
#if TARGET_OS_IPHONE
    unsigned numThreads = 30;
#else
    unsigned numThreads = 50;
#endif
    std::atomic_uint completed(numThreads);

    class TestWorker {
    public:
        TestWorker(ActorRef<TestWorker>, mbgl::AssetFileSource* fs_) : fs(fs_) {}

        void run(std::function<void()> endCallback) {
            const std::string asset("asset://nonempty");

            requestCallback = [this, asset, endCallback](mbgl::Response res) {
                EXPECT_EQ(nullptr, res.error);
                ASSERT_TRUE(res.data.get());
                EXPECT_EQ("content is here\n", *res.data);

                if (!--numRequests) {
                    endCallback();
                    request.reset();
                } else {
                    request = fs->request({ mbgl::Resource::Unknown, asset }, requestCallback);
                }
            };

            request = fs->request({ mbgl::Resource::Unknown, asset }, requestCallback);
        }

    private:
        unsigned numRequests = 1000;

        mbgl::AssetFileSource* fs;
        std::unique_ptr<mbgl::AsyncRequest> request;

        std::function<void(mbgl::Response)> requestCallback;
    };

    std::vector<std::unique_ptr<util::Thread<TestWorker>>> threads;

    for (unsigned i = 0; i < numThreads; ++i) {
        std::unique_ptr<util::Thread<TestWorker>> thread =
            std::make_unique<util::Thread<TestWorker>>("Test", &fs);

        thread->actor().invoke(&TestWorker::run, [&] { if (!--completed) loop.stop(); });
        threads.push_back(std::move(thread));
    }

    loop.run();
}

TEST(AssetFileSource, AcceptsURL) {
    EXPECT_TRUE(AssetFileSource::acceptsURL("asset://empty"));
    EXPECT_TRUE(AssetFileSource::acceptsURL("asset:///test"));
    EXPECT_FALSE(AssetFileSource::acceptsURL("assds://foo"));
    EXPECT_FALSE(AssetFileSource::acceptsURL("asset:"));
    EXPECT_FALSE(AssetFileSource::acceptsURL("style.json"));
    EXPECT_FALSE(AssetFileSource::acceptsURL(""));
}

TEST(AssetFileSource, EmptyFile) {
    util::RunLoop loop;

    AssetFileSource fs("test/fixtures/storage/assets");

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, "asset://empty" }, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("", *res.data);
        loop.stop();
    });

    loop.run();
}

TEST(AssetFileSource, NonEmptyFile) {
    util::RunLoop loop;

    AssetFileSource fs("test/fixtures/storage/assets");

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, "asset://nonempty" }, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("content is here\n", *res.data);
        loop.stop();
    });

    loop.run();
}

TEST(AssetFileSource, NonExistentFile) {
    util::RunLoop loop;

    AssetFileSource fs("test/fixtures/storage/assets");

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, "asset://does_not_exist" }, [&](Response res) {
        req.reset();
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        ASSERT_FALSE(res.data.get());
        // Do not assert on platform-specific error message.
        loop.stop();
    });

    loop.run();
}

TEST(AssetFileSource, InvalidURL) {
    util::RunLoop loop;

    AssetFileSource fs("test/fixtures/storage/assets");

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, "test://wrong-scheme" }, [&](Response res) {
        req.reset();
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::Other, res.error->reason);
        EXPECT_EQ("Invalid asset URL", res.error->message);
        ASSERT_FALSE(res.data.get());
        loop.stop();
    });

    loop.run();
}

TEST(AssetFileSource, ReadDirectory) {
    util::RunLoop loop;

    AssetFileSource fs("test/fixtures/storage/assets");

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, "asset://directory" }, [&](Response res) {
        req.reset();
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        ASSERT_FALSE(res.data.get());
        // Do not assert on platform-specific error message.
        loop.stop();
    });

    loop.run();
}

TEST(AssetFileSource, URLEncoding) {
    util::RunLoop loop;

    AssetFileSource fs("test/fixtures/storage/assets");

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, "asset://%6eonempty" }, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("content is here\n", *res.data);
        loop.stop();
    });

    loop.run();
}
#endif
