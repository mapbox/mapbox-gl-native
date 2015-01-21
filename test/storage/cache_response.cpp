#include "../util.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>
#include <mbgl/storage/default/sqlite_cache.hpp>

TEST(Storage, CacheResponse) {
    SCOPED_TEST(CacheResponse);

    using namespace mbgl;

    SQLiteCache cache(":memory:");
    DefaultFileSource fs(&cache, uv_default_loop());

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/cache" };

    fs.request(resource, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data, "Response 1");
        EXPECT_GT(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "");
        EXPECT_EQ(res.message, "");

        fs.request(resource, uv_default_loop(), [&, res](const Response &res2) {
            EXPECT_EQ(res2.status, res.status);
            EXPECT_EQ(res2.data, res.data);
            EXPECT_EQ(res2.expires, res.expires);
            EXPECT_EQ(res2.modified, res.modified);
            EXPECT_EQ(res2.etag, res.etag);
            EXPECT_EQ(res2.message, res.message);

            CacheResponse.finish();
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
