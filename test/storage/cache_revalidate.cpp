#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

TEST_F(Storage, CacheRevalidateSame) {
    SCOPED_TEST(CacheRevalidateSame)

    using namespace mbgl;

    SQLiteCache cache(":memory:");
    DefaultFileSource fs(&cache);

    const Resource revalidateSame { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    Request* req1 = nullptr;
    Request* req2 = nullptr;
    req1 = fs.request(revalidateSame, uv_default_loop(), [&](const Response &res) {
        // This callback can get triggered multiple times. We only care about the first invocation.
        // It will get triggered again when refreshing the req2 (see below).
        static bool first = true;
        if (!first) {
            return;
        }
        first = false;

        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("snowfall", res.etag);
        EXPECT_EQ("", res.message);

        req2 = fs.request(revalidateSame, uv_default_loop(), [&, res](const Response &res2) {
            if (res2.stale) {
                // Discard stale responses, if any.
                return;
            }

            ASSERT_TRUE(req1);
            fs.cancel(req1);
            req1 = nullptr;

            ASSERT_TRUE(req2);
            fs.cancel(req2);
            req2 = nullptr;

            EXPECT_EQ(Response::Successful, res2.status);
            EXPECT_EQ(false, res.stale);
            ASSERT_TRUE(res2.data.get());
            EXPECT_EQ(res.data, res2.data);
            EXPECT_EQ("Response", *res2.data);
            // We use this to indicate that a 304 reply came back.
            EXPECT_LT(0, res2.expires);
            EXPECT_EQ(0, res2.modified);
            // We're not sending the ETag in the 304 reply, but it should still be there.
            EXPECT_EQ("snowfall", res2.etag);
            EXPECT_EQ("", res2.message);

            CacheRevalidateSame.finish();
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, CacheRevalidateModified) {
    SCOPED_TEST(CacheRevalidateModified)

    using namespace mbgl;

    SQLiteCache cache(":memory:");
    DefaultFileSource fs(&cache);

    const Resource revalidateModified{ Resource::Unknown,
                                       "http://127.0.0.1:3000/revalidate-modified" };
    Request* req1 = nullptr;
    Request* req2 = nullptr;
    req1 = fs.request(revalidateModified, uv_default_loop(), [&](const Response& res) {
        // This callback can get triggered multiple times. We only care about the first invocation.
        // It will get triggered again when refreshing the req2 (see below).
        static bool first = true;
        if (!first) {
            return;
        }
        first = false;

        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(1420070400, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);

        req2 = fs.request(revalidateModified, uv_default_loop(), [&, res](const Response &res2) {
            if (res2.stale) {
                // Discard stale responses, if any.
                return;
            }

            ASSERT_TRUE(req1);
            fs.cancel(req1);
            req1 = nullptr;

            ASSERT_TRUE(req2);
            fs.cancel(req2);
            req2 = nullptr;

            EXPECT_EQ(Response::Successful, res2.status);
            EXPECT_EQ(false, res2.stale);
            ASSERT_TRUE(res2.data.get());
            EXPECT_EQ("Response", *res2.data);
            EXPECT_EQ(res.data, res2.data);
            // We use this to indicate that a 304 reply came back.
            EXPECT_LT(0, res2.expires);
            EXPECT_EQ(1420070400, res2.modified);
            EXPECT_EQ("", res2.etag);
            EXPECT_EQ("", res2.message);

            CacheRevalidateModified.finish();
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, CacheRevalidateEtag) {
    SCOPED_TEST(CacheRevalidateEtag)

    using namespace mbgl;

    SQLiteCache cache(":memory:");
    DefaultFileSource fs(&cache);

    const Resource revalidateEtag { Resource::Unknown, "http://127.0.0.1:3000/revalidate-etag" };
    Request* req1 = nullptr;
    Request* req2 = nullptr;
    req1 = fs.request(revalidateEtag, uv_default_loop(), [&](const Response &res) {
        // This callback can get triggered multiple times. We only care about the first invocation.
        // It will get triggered again when refreshing the req2 (see below).
        static bool first = true;
        if (!first) {
            return;
        }
        first = false;

        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response 1", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("response-1", res.etag);
        EXPECT_EQ("", res.message);

        req2 = fs.request(revalidateEtag, uv_default_loop(), [&, res](const Response &res2) {
            if (res2.stale) {
                // Discard stale responses, if any.
                return;
            }

            ASSERT_TRUE(req1);
            fs.cancel(req1);
            req1 = nullptr;

            ASSERT_TRUE(req2);
            fs.cancel(req2);
            req2 = nullptr;

            EXPECT_EQ(Response::Successful, res2.status);
            EXPECT_EQ(false, res.stale);
            ASSERT_TRUE(res2.data.get());
            EXPECT_NE(res.data, res2.data);
            EXPECT_EQ("Response 2", *res2.data);
            EXPECT_EQ(0, res2.expires);
            EXPECT_EQ(0, res2.modified);
            EXPECT_EQ("response-2", res2.etag);
            EXPECT_EQ("", res2.message);

            CacheRevalidateEtag.finish();
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
