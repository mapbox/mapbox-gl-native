#include "storage.hpp"

#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread.hpp>

namespace {

std::string getFileSourceRoot() {
#ifdef MBGL_ASSET_ZIP
    return "test/fixtures/storage/assets.zip";
#else
    return "";
#endif
}

class TestWorker {
public:
    TestWorker(mbgl::OnlineFileSource* fs_) : fs(fs_) {}

    void run(std::function<void()> endCallback) {
        const std::string asset("asset://TEST_DATA/fixtures/storage/nonempty");

        requestCallback = [this, asset, endCallback](mbgl::Response res) {
            EXPECT_EQ(nullptr, res.error);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("content is here\n", *res.data);

            if (res.stale) {
                return;
            }

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

    mbgl::OnlineFileSource* fs;
    std::unique_ptr<mbgl::FileRequest> request;

    std::function<void(mbgl::Response)> requestCallback;
};

} // namespace

TEST_F(Storage, AssetStress) {
    SCOPED_TEST(AssetStress)

    using namespace mbgl;

    util::RunLoop loop;

    mbgl::OnlineFileSource fs(nullptr, getFileSourceRoot());

    unsigned numThreads = 50;

    auto callback = [&] {
        if (!--numThreads) {
            loop.stop();
        }
    };

    std::vector<std::unique_ptr<util::Thread<TestWorker>>> threads;
    std::vector<std::unique_ptr<mbgl::WorkRequest>> requests;
    util::ThreadContext context = { "Test", util::ThreadType::Map, util::ThreadPriority::Regular };

    for (unsigned i = 0; i < numThreads; ++i) {
        std::unique_ptr<util::Thread<TestWorker>> thread =
            std::make_unique<util::Thread<TestWorker>>(context, &fs);

        requests.push_back(
            thread->invokeWithCallback(&TestWorker::run, callback));

        threads.push_back(std::move(thread));
    }

    loop.run();

    AssetStress.finish();
}

TEST_F(Storage, AssetEmptyFile) {
    SCOPED_TEST(EmptyFile)

    using namespace mbgl;

    util::RunLoop loop;

    OnlineFileSource fs(nullptr, getFileSourceRoot());

    std::unique_ptr<FileRequest> req = fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage/empty" }, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("", *res.data);
        EXPECT_EQ(Seconds::zero(), res.expires);
        EXPECT_LT(1420000000, res.modified.count());
        EXPECT_NE("", res.etag);
        loop.stop();
        EmptyFile.finish();
    });

    loop.run();
}

TEST_F(Storage, AssetNonEmptyFile) {
    SCOPED_TEST(NonEmptyFile)

    using namespace mbgl;

    util::RunLoop loop;

    OnlineFileSource fs(nullptr, getFileSourceRoot());

    std::unique_ptr<FileRequest> req = fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage/nonempty" }, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("content is here\n", *res.data);
        EXPECT_EQ(Seconds::zero(), res.expires);
        EXPECT_LT(1420000000, res.modified.count());
        EXPECT_NE("", res.etag);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("content is here\n", *res.data);
        loop.stop();
        NonEmptyFile.finish();
    });

    loop.run();
}

TEST_F(Storage, AssetNonExistentFile) {
    SCOPED_TEST(NonExistentFile)

    using namespace mbgl;

    util::RunLoop loop;

    OnlineFileSource fs(nullptr, getFileSourceRoot());

    std::unique_ptr<FileRequest> req = fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage/does_not_exist" }, [&](Response res) {
        req.reset();
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        EXPECT_EQ(false, res.stale);
        ASSERT_FALSE(res.data.get());
        EXPECT_EQ(Seconds::zero(), res.expires);
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);
#ifdef MBGL_ASSET_ZIP
        EXPECT_EQ("Could not stat file in zip archive", res.error->message);
#elif MBGL_ASSET_FS
        EXPECT_EQ("No such file or directory", res.error->message);
#endif
        loop.stop();
        NonExistentFile.finish();
    });

    loop.run();
}
