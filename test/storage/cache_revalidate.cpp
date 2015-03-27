#include "storage.hpp"

#include <mbgl/map/environment.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/default/sqlite_cache.hpp>

#include <uv.h>

#include <thread>

TEST_F(Storage, CacheRevalidate) {
    SCOPED_TEST(CacheRevalidateSame)
    SCOPED_TEST(CacheRevalidateModified)
    SCOPED_TEST(CacheRevalidateEtag)

    using namespace mbgl;

    SQLiteCache cache(":memory:");
    DefaultFileSource fs(&cache);

    Environment env(fs);
    EnvironmentScope scope(env, ThreadType::Test, TEST_CASE_NAME(), uv_default_loop());

    const Resource revalidateSame { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    fs.request(revalidateSame, std::this_thread::get_id(), [&](const Response &res) {
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ("Response", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("snowfall", res.etag);
        EXPECT_EQ("", res.message);

        fs.request(revalidateSame, std::this_thread::get_id(), [&, res](const Response &res2) {
            EXPECT_EQ(Response::Successful, res2.status);
            EXPECT_EQ("Response", res2.data);
            // We use this to indicate that a 304 reply came back.
            EXPECT_LT(0, res2.expires);
            EXPECT_EQ(0, res2.modified);
            // We're not sending the ETag in the 304 reply, but it should still be there.
            EXPECT_EQ("snowfall", res2.etag);
            EXPECT_EQ("", res2.message);

            CacheRevalidateSame.finish();
        });
    });

    const Resource revalidateModified{ Resource::Unknown,
                                       "http://127.0.0.1:3000/revalidate-modified" };
    fs.request(revalidateModified, std::this_thread::get_id(), [&](const Response &res) {
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ("Response", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(1420070400, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);

        fs.request(revalidateModified, std::this_thread::get_id(), [&, res](const Response &res2) {
            EXPECT_EQ(Response::Successful, res2.status);
            EXPECT_EQ("Response", res2.data);
            // We use this to indicate that a 304 reply came back.
            EXPECT_LT(0, res2.expires);
            EXPECT_EQ(1420070400, res2.modified);
            EXPECT_EQ("", res2.etag);
            EXPECT_EQ("", res2.message);

            CacheRevalidateModified.finish();
        });
    });

    const Resource revalidateEtag { Resource::Unknown, "http://127.0.0.1:3000/revalidate-etag" };
    fs.request(revalidateEtag, std::this_thread::get_id(), [&](const Response &res) {
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ("Response 1", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("response-1", res.etag);
        EXPECT_EQ("", res.message);

        fs.request(revalidateEtag, std::this_thread::get_id(), [&, res](const Response &res2) {
            EXPECT_EQ(Response::Successful, res2.status);
            EXPECT_EQ("Response 2", res2.data);
            EXPECT_EQ(0, res2.expires);
            EXPECT_EQ(0, res2.modified);
            EXPECT_EQ("response-2", res2.etag);
            EXPECT_EQ("", res2.message);

            CacheRevalidateEtag.finish();
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
