#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, CacheRevalidateSame) {
    SCOPED_TEST(CacheRevalidateSame)

    using namespace mbgl;

    SQLiteCache cache(":memory:");
    DefaultFileSource fs(&cache);
    util::RunLoop loop(uv_default_loop());

    const Resource revalidateSame { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<FileRequest> req2;
    req1 = fs.request(revalidateSame, [&](Response res) {
        // This callback can get triggered multiple times. We only care about the first invocation.
        // It will get triggered again when refreshing the req2 (see below).
        static bool first = true;
        if (!first) {
            return;
        }
        first = false;

        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("snowfall", res.etag);

        req2 = fs.request(revalidateSame, [&, res](Response res2) {
            if (res2.stale) {
                // Discard stale responses, if any.
                return;
            }

            ASSERT_TRUE(req1.get());
            req1.reset();

            ASSERT_TRUE(req2.get());
            req2.reset();

            EXPECT_EQ(nullptr, res2.error);
            EXPECT_EQ(false, res2.stale);
            ASSERT_TRUE(res2.data.get());
            EXPECT_EQ(res.data, res2.data);
            EXPECT_EQ("Response", *res2.data);
            // We use this to indicate that a 304 reply came back.
            EXPECT_LT(0, res2.expires);
            EXPECT_EQ(0, res2.modified);
            // We're not sending the ETag in the 304 reply, but it should still be there.
            EXPECT_EQ("snowfall", res2.etag);

            loop.stop();
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
    util::RunLoop loop(uv_default_loop());

    const Resource revalidateModified{ Resource::Unknown,
                                       "http://127.0.0.1:3000/revalidate-modified" };
    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<FileRequest> req2;
    req1 = fs.request(revalidateModified, [&](Response res) {
        // This callback can get triggered multiple times. We only care about the first invocation.
        // It will get triggered again when refreshing the req2 (see below).
        static bool first = true;
        if (!first) {
            return;
        }
        first = false;

        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(1420070400, res.modified);
        EXPECT_EQ("", res.etag);

        req2 = fs.request(revalidateModified, [&, res](Response res2) {
            if (res2.stale) {
                // Discard stale responses, if any.
                return;
            }

            ASSERT_TRUE(req1.get());
            req1.reset();

            ASSERT_TRUE(req2.get());
            req2.reset();

            EXPECT_EQ(nullptr, res2.error);
            EXPECT_EQ(false, res2.stale);
            ASSERT_TRUE(res2.data.get());
            EXPECT_EQ("Response", *res2.data);
            EXPECT_EQ(res.data, res2.data);
            // We use this to indicate that a 304 reply came back.
            EXPECT_LT(0, res2.expires);
            EXPECT_EQ(1420070400, res2.modified);
            EXPECT_EQ("", res2.etag);

            loop.stop();
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
    util::RunLoop loop(uv_default_loop());

    const Resource revalidateEtag { Resource::Unknown, "http://127.0.0.1:3000/revalidate-etag" };
    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<FileRequest> req2;
    req1 = fs.request(revalidateEtag, [&](Response res) {
        // This callback can get triggered multiple times. We only care about the first invocation.
        // It will get triggered again when refreshing the req2 (see below).
        static bool first = true;
        if (!first) {
            return;
        }
        first = false;

        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response 1", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("response-1", res.etag);

        req2 = fs.request(revalidateEtag, [&, res](Response res2) {
            if (res2.stale) {
                // Discard stale responses, if any.
                return;
            }

            ASSERT_TRUE(req1.get());
            req1.reset();

            ASSERT_TRUE(req2.get());
            req2.reset();

            EXPECT_EQ(nullptr, res2.error);
            EXPECT_EQ(false, res2.stale);
            ASSERT_TRUE(res2.data.get());
            EXPECT_NE(res.data, res2.data);
            EXPECT_EQ("Response 2", *res2.data);
            EXPECT_EQ(0, res2.expires);
            EXPECT_EQ(0, res2.modified);
            EXPECT_EQ("response-2", res2.etag);

            loop.stop();
            CacheRevalidateEtag.finish();
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
