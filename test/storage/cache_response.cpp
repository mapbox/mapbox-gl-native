#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

TEST_F(Storage, CacheResponse) {
    SCOPED_TEST(CacheResponse);

    using namespace mbgl;

    SQLiteCache cache(":memory:");
    DefaultFileSource fs(&cache);

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/cache" };
    Response response;

    Request* req = fs.request(resource, uv_default_loop(), [&](const Response &res) {
        fs.cancel(req);
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response 1", *res.data);
        EXPECT_LT(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        response = res;
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    // Now test that we get the same values as in the previous request. If we'd go to the server
    // again, we'd get different values.
    req = fs.request(resource, uv_default_loop(), [&](const Response &res) {
        fs.cancel(req);
        EXPECT_EQ(response.error, res.error);
        EXPECT_EQ(response.stale, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ(*response.data, *res.data);
        EXPECT_EQ(response.expires, res.expires);
        EXPECT_EQ(response.modified, res.modified);
        EXPECT_EQ(response.etag, res.etag);

        CacheResponse.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
