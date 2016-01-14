#include "storage.hpp"

#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, CacheResponse) {
    SCOPED_TEST(CacheResponse);

    using namespace mbgl;

    util::RunLoop loop;
    SQLiteCache cache(":memory:");
    OnlineFileSource fs(&cache);

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

// Make sure we /do/ store 404 Not Found responses into the cache
TEST_F(Storage, CacheNotFound) {
    SCOPED_TEST(CacheNotFound);

    using namespace mbgl;

    util::RunLoop loop;
    SQLiteCache cache(":memory:");
    OnlineFileSource fs(&cache);

    const Resource resource{ Resource::Unknown, "http://127.0.0.1:3000/not-found" };

    // Insert existing data into the cache that will be marked as stale.
    Response response;
    response.data = std::make_shared<const std::string>("existing data");
    cache.put(resource, response, SQLiteCache::Hint::Full);

    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<WorkRequest> req2;

    int counter = 0;

    // Then, request the actual URL and check that we're getting the rigged cache response first,
    // then the connection error message.
    req1 = fs.request(resource, [&](Response res) {
        if (counter == 0) {
            EXPECT_EQ(nullptr, res.error);
            EXPECT_EQ(true, res.stale);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("existing data", *res.data);
            EXPECT_EQ(Seconds::zero(), res.expires);
            EXPECT_EQ(Seconds::zero(), res.modified);
            EXPECT_EQ("", res.etag);
        } else if (counter == 1) {
            EXPECT_NE(nullptr, res.error);
            EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("Not Found!", *res.data);
            req1.reset();

            // Finally, check the cache to make sure we cached the 404 response.
            req2 = cache.get(resource, [&](std::unique_ptr<Response> res2) {
                EXPECT_NE(nullptr, res2->error);
                EXPECT_EQ(Response::Error::Reason::NotFound, res2->error->reason);
                ASSERT_TRUE(res2->data.get());
                EXPECT_EQ("Not Found!", *res2->data);
                CacheNotFound.finish();
                loop.stop();
            });
        } else {
            FAIL() << "Got too many responses";
        }
        counter++;
    });

    loop.run();
}

// Make sure we don't store a connection error into the cache
TEST_F(Storage, DontCacheConnectionErrors) {
    SCOPED_TEST(DontCacheConnectionErrors);

    using namespace mbgl;

    util::RunLoop loop;
    SQLiteCache cache(":memory:");
    OnlineFileSource fs(&cache);

    const Resource resource{ Resource::Unknown, "http://127.0.0.1:3001" };

    // Insert existing data into the cache that will be marked as stale.
    Response response;
    response.data = std::make_shared<const std::string>("existing data");
    cache.put(resource, response, SQLiteCache::Hint::Full);

    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<WorkRequest> req2;

    int counter = 0;

    // Then, request the actual URL and check that we're getting the rigged cache response first,
    // then the connection error message.
    req1 = fs.request(resource, [&](Response res) {
        if (counter == 0) {
            EXPECT_EQ(nullptr, res.error);
            EXPECT_EQ(true, res.stale);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("existing data", *res.data);
            EXPECT_EQ(Seconds::zero(), res.expires);
            EXPECT_EQ(Seconds::zero(), res.modified);
            EXPECT_EQ("", res.etag);
        } else if (counter == 1) {
            EXPECT_NE(nullptr, res.error);
            EXPECT_EQ(Response::Error::Reason::Connection, res.error->reason);
            req1.reset();

            // Finally, check the cache to make sure we still have our original data in there rather
            // than the failed connection attempt.
            req2 = cache.get(resource, [&](std::unique_ptr<Response> res2) {
                EXPECT_EQ(nullptr, res2->error);
                ASSERT_TRUE(res2->data.get());
                EXPECT_EQ("existing data", *res2->data);
                DontCacheConnectionErrors.finish();
                loop.stop();
            });
        } else {
            FAIL() << "Got too many responses";
        }
        counter++;
    });

    loop.run();
}

// Make sure we don't store a bad server response into the cache
TEST_F(Storage, DontCacheServerErrors) {
    SCOPED_TEST(DontCacheServerErrors);

    using namespace mbgl;

    util::RunLoop loop;
    SQLiteCache cache(":memory:");
    OnlineFileSource fs(&cache);

    const Resource resource{ Resource::Unknown, "http://127.0.0.1:3000/permanent-error" };

    // Insert existing data into the cache that will be marked as stale.
    Response response;
    response.data = std::make_shared<const std::string>("existing data");
    cache.put(resource, response, SQLiteCache::Hint::Full);

    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<WorkRequest> req2;

    int counter = 0;

    // Then, request the actual URL and check that we're getting the rigged cache response first,
    // then the server error message.
    req1 = fs.request(resource, [&](Response res) {
        if (counter == 0) {
            EXPECT_EQ(nullptr, res.error);
            EXPECT_EQ(true, res.stale);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("existing data", *res.data);
            EXPECT_EQ(Seconds::zero(), res.expires);
            EXPECT_EQ(Seconds::zero(), res.modified);
            EXPECT_EQ("", res.etag);
        } else if (counter == 1) {
            EXPECT_NE(nullptr, res.error);
            EXPECT_EQ(Response::Error::Reason::Server, res.error->reason);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("Server Error!", *res.data);
            req1.reset();

            // Finally, check the cache to make sure we still have our original data in there rather
            // than the failed connection attempt.
            req2 = cache.get(resource, [&](std::unique_ptr<Response> res2) {
                EXPECT_EQ(nullptr, res2->error);
                ASSERT_TRUE(res2->data.get());
                EXPECT_EQ("existing data", *res2->data);
                DontCacheServerErrors.finish();
                loop.stop();
            });
        } else {
            FAIL() << "Got too many responses";
        }
        counter++;
    });

    loop.run();
}
