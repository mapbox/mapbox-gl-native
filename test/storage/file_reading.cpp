#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, AssetEmptyFile) {
    SCOPED_TEST(EmptyFile)

    using namespace mbgl;

#ifdef MBGL_ASSET_ZIP
    DefaultFileSource fs(nullptr, "test/fixtures/storage/assets.zip");
#else
    DefaultFileSource fs(nullptr);
#endif

    util::RunLoop loop(uv_default_loop());

    std::unique_ptr<FileRequest> req = fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage/empty" }, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_LT(1420000000, res.modified);
        EXPECT_NE("", res.etag);
        loop.stop();
        EmptyFile.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, AssetNonEmptyFile) {
    SCOPED_TEST(NonEmptyFile)

    using namespace mbgl;

#ifdef MBGL_ASSET_ZIP
    DefaultFileSource fs(nullptr, "test/fixtures/storage/assets.zip");
#else
    DefaultFileSource fs(nullptr);
#endif

    util::RunLoop loop(uv_default_loop());

    std::unique_ptr<FileRequest> req = fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage/nonempty" }, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("content is here\n", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_LT(1420000000, res.modified);
        EXPECT_NE("", res.etag);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("content is here\n", *res.data);
        loop.stop();
        NonEmptyFile.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, AssetNonExistentFile) {
    SCOPED_TEST(NonExistentFile)

    using namespace mbgl;

#ifdef MBGL_ASSET_ZIP
    DefaultFileSource fs(nullptr, "test/fixtures/storage/assets.zip");
#else
    DefaultFileSource fs(nullptr);
#endif

    util::RunLoop loop(uv_default_loop());

    std::unique_ptr<FileRequest> req = fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage/does_not_exist" }, [&](Response res) {
        req.reset();
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        EXPECT_EQ(false, res.stale);
        ASSERT_FALSE(res.data.get());
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
#ifdef MBGL_ASSET_ZIP
        EXPECT_EQ("No such file", res.error->message);
#elif MBGL_ASSET_FS
        EXPECT_EQ("no such file or directory", res.error->message);
#endif
        loop.stop();
        NonExistentFile.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
