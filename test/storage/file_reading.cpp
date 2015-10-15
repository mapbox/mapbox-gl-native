#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/platform/platform.hpp>

TEST_F(Storage, AssetEmptyFile) {
    SCOPED_TEST(EmptyFile)

    using namespace mbgl;

#ifdef MBGL_ASSET_ZIP
    DefaultFileSource fs(nullptr, "test/fixtures/storage/assets.zip");
#else
    DefaultFileSource fs(nullptr);
#endif

    Request* req = fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage/empty" }, uv_default_loop(),
               [&](const Response &res) {
        fs.cancel(req);
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ(0ul, res.data.size());
        EXPECT_EQ(0, res.expires);
        EXPECT_LT(1420000000, res.modified);
        EXPECT_NE("", res.etag);
        EXPECT_EQ("", res.message);
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

    Request* req = fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage/nonempty" },
               uv_default_loop(), [&](const Response &res) {
        fs.cancel(req);
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ(16ul, res.data.size());
        EXPECT_EQ(0, res.expires);
        EXPECT_LT(1420000000, res.modified);
        EXPECT_NE("", res.etag);
        EXPECT_EQ("", res.message);
        EXPECT_EQ("content is here\n", res.data);
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

    Request* req = fs.request({ Resource::Unknown, "asset://TEST_DATA/fixtures/storage/does_not_exist" },
               uv_default_loop(), [&](const Response &res) {
        fs.cancel(req);
        EXPECT_EQ(Response::Error, res.status);
        EXPECT_EQ(0ul, res.data.size());
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
#ifdef MBGL_ASSET_ZIP
        EXPECT_EQ("No such file", res.message);
#elif MBGL_ASSET_FS
        EXPECT_EQ("no such file or directory", res.message);
#endif
        NonExistentFile.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
