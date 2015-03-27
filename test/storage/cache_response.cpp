#include "storage.hpp"

#include <mbgl/map/environment.hpp>
#include <mbgl/storage/default/sqlite_cache.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <uv.h>

#include <thread>

TEST_F(Storage, CacheResponse) {
    SCOPED_TEST(CacheResponse);

    using namespace mbgl;

    SQLiteCache cache(":memory:");
    DefaultFileSource fs(&cache);

    Environment env(fs);
    EnvironmentScope scope(env, ThreadType::Test, TEST_CASE_NAME(), uv_default_loop());

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/cache" };

    fs.request(resource, std::this_thread::get_id(), [&](const Response &res) {
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ("Response 1", res.data);
        EXPECT_LT(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);

        fs.request(resource, std::this_thread::get_id(), [&, res](const Response &res2) {
            EXPECT_EQ(res.status, res2.status);
            EXPECT_EQ(res.data, res2.data);
            EXPECT_EQ(res.expires, res2.expires);
            EXPECT_EQ(res.modified, res2.modified);
            EXPECT_EQ(res.etag, res2.etag);
            EXPECT_EQ(res.message, res2.message);

            CacheResponse.finish();
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
