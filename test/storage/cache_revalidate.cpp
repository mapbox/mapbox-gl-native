#include "../util.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>
#include <mbgl/storage/default/sqlite_cache.hpp>

TEST(Storage, CacheRevalidate) {
    SCOPED_TEST(CacheRevalidateSame)
    SCOPED_TEST(CacheRevalidateModified)
    SCOPED_TEST(CacheRevalidateEtag)

    using namespace mbgl;

    SQLiteCache cache(":memory:");
    DefaultFileSource fs(&cache);

    const Resource revalidateSame { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    fs.request(revalidateSame, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data, "Response");
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "snowfall");
        EXPECT_EQ(res.message, "");

        fs.request(revalidateSame, uv_default_loop(), [&, res](const Response &res2) {
            EXPECT_EQ(res2.status, Response::Successful);
            EXPECT_EQ(res2.data, "Response");
            // We use this to indicate that a 304 reply came back.
            EXPECT_GT(res2.expires, 0);
            EXPECT_EQ(res2.modified, 0);
            // We're not sending the ETag in the 304 reply, but it should still be there.
            EXPECT_EQ(res2.etag, "snowfall");
            EXPECT_EQ(res2.message, "");

            CacheRevalidateSame.finish();
        });
    });

    const Resource revalidateModified { Resource::Unknown, "http://127.0.0.1:3000/revalidate-modified" };
    fs.request(revalidateModified, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data, "Response");
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 1420070400);
        EXPECT_EQ(res.etag, "");
        EXPECT_EQ(res.message, "");

        fs.request(revalidateModified, uv_default_loop(), [&, res](const Response &res2) {
            EXPECT_EQ(res2.status, Response::Successful);
            EXPECT_EQ(res2.data, "Response");
            // We use this to indicate that a 304 reply came back.
            EXPECT_GT(res2.expires, 0);
            EXPECT_EQ(res2.modified, 1420070400);
            EXPECT_EQ(res2.etag, "");
            EXPECT_EQ(res2.message, "");

            CacheRevalidateModified.finish();
        });
    });

    const Resource revalidateEtag { Resource::Unknown, "http://127.0.0.1:3000/revalidate-etag" };
    fs.request(revalidateEtag, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data, "Response 1");
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "response-1");
        EXPECT_EQ(res.message, "");

        fs.request(revalidateEtag, uv_default_loop(), [&, res](const Response &res2) {
            EXPECT_EQ(res2.status, Response::Successful);
            EXPECT_EQ(res2.data, "Response 2");
            EXPECT_EQ(res2.expires, 0);
            EXPECT_EQ(res2.modified, 0);
            EXPECT_EQ(res2.etag, "response-2");
            EXPECT_EQ(res2.message, "");

            CacheRevalidateEtag.finish();
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
