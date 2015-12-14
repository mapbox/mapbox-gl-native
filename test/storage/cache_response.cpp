#include "storage.hpp"

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, CacheResponse) {
    SCOPED_TEST(CacheResponse);

    using namespace mbgl;

    util::RunLoop loop;
    SQLiteCache cache(":memory:");
    DefaultFileSource fs(&cache);

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/cache" };
    Response response;

    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<FileRequest> req2;

    req1 = fs.request(resource, [&](Response res) {
        req1.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response 1", *res.data);
        EXPECT_LT(Seconds::zero(), res.expires);
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);
        response = res;

        // Now test that we get the same values as in the previous request. If we'd go to the server
        // again, we'd get different values.
        req2 = fs.request(resource, [&](Response res2) {
            req2.reset();
            EXPECT_EQ(response.error, res2.error);
            EXPECT_EQ(response.stale, res2.stale);
            ASSERT_TRUE(res2.data.get());
            EXPECT_EQ(*response.data, *res2.data);
            EXPECT_EQ(response.expires, res2.expires);
            EXPECT_EQ(response.modified, res2.modified);
            EXPECT_EQ(response.etag, res2.etag);

            loop.stop();
            CacheResponse.finish();
        });
    });

    loop.run();
}
