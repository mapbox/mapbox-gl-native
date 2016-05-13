#include <mbgl/test/util.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;

TEST(DefaultFileSource, TEST_REQUIRES_SERVER(CacheResponse)) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/cache" };
    Response response;

    std::unique_ptr<AsyncRequest> req1;
    std::unique_ptr<AsyncRequest> req2;

    req1 = fs.request(resource, [&](Response res) {
        req1.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response 1", *res.data);
        EXPECT_TRUE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        response = res;

        // Now test that we get the same values as in the previous request. If we'd go to the server
        // again, we'd get different values.
        req2 = fs.request(resource, [&](Response res2) {
            req2.reset();
            EXPECT_EQ(response.error, res2.error);
            ASSERT_TRUE(res2.data.get());
            EXPECT_EQ(*response.data, *res2.data);
            EXPECT_EQ(response.expires, res2.expires);
            EXPECT_EQ(response.modified, res2.modified);
            EXPECT_EQ(response.etag, res2.etag);

            loop.stop();
        });
    });

    loop.run();
}

TEST(DefaultFileSource, TEST_REQUIRES_SERVER(CacheRevalidateSame)) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource revalidateSame { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    std::unique_ptr<AsyncRequest> req1;
    std::unique_ptr<AsyncRequest> req2;
    uint16_t counter = 0;

    // First request causes the response to get cached.
    req1 = fs.request(revalidateSame, [&](Response res) {
        req1.reset();

        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_EQ("snowfall", *res.etag);

        // Second request returns the cached response, then immediately revalidates.
        req2 = fs.request(revalidateSame, [&, res](Response res2) {
            if (counter == 0) {
                ++counter;
                EXPECT_FALSE(res2.notModified);
            } else {
                req2.reset();

                EXPECT_EQ(nullptr, res2.error);
                EXPECT_TRUE(res2.notModified);
                ASSERT_FALSE(res2.data.get());
                EXPECT_TRUE(bool(res2.expires));
                EXPECT_FALSE(bool(res2.modified));
                // We're not sending the ETag in the 304 reply, but it should still be there.
                EXPECT_EQ("snowfall", *res2.etag);

                loop.stop();
            }
        });
    });

    loop.run();
}

TEST(DefaultFileSource, TEST_REQUIRES_SERVER(CacheRevalidateModified)) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource revalidateModified{ Resource::Unknown,
                                       "http://127.0.0.1:3000/revalidate-modified" };
    std::unique_ptr<AsyncRequest> req1;
    std::unique_ptr<AsyncRequest> req2;
    uint16_t counter = 0;

    // First request causes the response to get cached.
    req1 = fs.request(revalidateModified, [&](Response res) {
        req1.reset();

        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_EQ(Timestamp{ Seconds(1420070400) }, *res.modified);
        EXPECT_FALSE(res.etag);

        // Second request returns the cached response, then immediately revalidates.
        req2 = fs.request(revalidateModified, [&, res](Response res2) {
            if (counter == 0) {
                ++counter;
                EXPECT_FALSE(res2.notModified);
            } else {
                req2.reset();

                EXPECT_EQ(nullptr, res2.error);
                EXPECT_TRUE(res2.notModified);
                ASSERT_FALSE(res2.data.get());
                EXPECT_TRUE(bool(res2.expires));
                EXPECT_EQ(Timestamp{ Seconds(1420070400) }, *res2.modified);
                EXPECT_FALSE(res2.etag);

                loop.stop();
            }
        });
    });

    loop.run();
}

TEST(DefaultFileSource, TEST_REQUIRES_SERVER(CacheRevalidateEtag)) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource revalidateEtag { Resource::Unknown, "http://127.0.0.1:3000/revalidate-etag" };
    std::unique_ptr<AsyncRequest> req1;
    std::unique_ptr<AsyncRequest> req2;
    uint16_t counter = 0;

    // First request causes the response to get cached.
    req1 = fs.request(revalidateEtag, [&](Response res) {
        req1.reset();

        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response 1", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_EQ("response-1", *res.etag);

        // Second request returns the cached response, then immediately revalidates.
        req2 = fs.request(revalidateEtag, [&, res](Response res2) {
            if (counter == 0) {
                ++counter;
                EXPECT_FALSE(res2.notModified);
            } else {
                req2.reset();

                EXPECT_EQ(nullptr, res2.error);
                ASSERT_TRUE(res2.data.get());
                EXPECT_NE(res.data, res2.data);
                EXPECT_EQ("Response 2", *res2.data);
                EXPECT_FALSE(bool(res2.expires));
                EXPECT_FALSE(bool(res2.modified));
                EXPECT_EQ("response-2", *res2.etag);

                loop.stop();
            }
        });
    });

    loop.run();
}

// Test for https://github.com/mapbox/mapbox-gl-native/issue/1369
//
// A request for http://example.com is made. This triggers a cache get. While the cache get is
// pending, the request is canceled. This removes it from pending. Then, still while the cache get
// is pending, a second request is made for the same resource. This adds an entry back to pending
// and queues another cache request, even though the first one is still pending. Now both cache
// requests resolve to misses, resulting in two HTTP requests for the same resource. The first one
// will notify as expected, the second one will have bound a DefaultFileRequest* in the lambda that
// gets invalidated by the first notify's pending.erase, and when it gets notified, the crash
// occurs.

TEST(DefaultFileSource, TEST_REQUIRES_SERVER(HTTPIssue1369)) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };

    auto req = fs.request(resource, [&](Response) {
        ADD_FAILURE() << "Callback should not be called";
    });
    req.reset();
    req = fs.request(resource, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}
