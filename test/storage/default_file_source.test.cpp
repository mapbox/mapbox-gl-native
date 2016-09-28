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

TEST(DefaultFileSource, OptionalNonExpired) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource optionalResource { Resource::Unknown, "http://127.0.0.1:3000/test", {}, Resource::Optional };

    using namespace std::chrono_literals;

    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;
    fs.put(optionalResource, response);

    std::unique_ptr<AsyncRequest> req;
    req = fs.request(optionalResource, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Cached value", *res.data);
        ASSERT_TRUE(bool(res.expires));
        EXPECT_EQ(*response.expires, *res.expires);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

TEST(DefaultFileSource, OptionalExpired) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource optionalResource { Resource::Unknown, "http://127.0.0.1:3000/test", {}, Resource::Optional };

    using namespace std::chrono_literals;

    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() - 1h;
    fs.put(optionalResource, response);

    std::unique_ptr<AsyncRequest> req;
    req = fs.request(optionalResource, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Cached value", *res.data);
        ASSERT_TRUE(bool(res.expires));
        EXPECT_EQ(*response.expires, *res.expires);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

TEST(DefaultFileSource, OptionalNotFound) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource optionalResource { Resource::Unknown, "http://127.0.0.1:3000/test", {}, Resource::Optional };

    using namespace std::chrono_literals;

    std::unique_ptr<AsyncRequest> req;
    req = fs.request(optionalResource, [&](Response res) {
        req.reset();
        ASSERT_TRUE(res.error.get());
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        EXPECT_EQ("Not found in offline database", res.error->message);
        EXPECT_FALSE(res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

// Test that we can make a request with etag data that doesn't first try to load
// from cache like a regular request
TEST(DefaultFileSource, TEST_REQUIRES_SERVER(NoCacheRefreshEtagNotModified)) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    Resource resource { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    resource.priorEtag.emplace("snowfall");

    using namespace std::chrono_literals;

    // Put a fake value into the cache to make sure we're not retrieving anything from the cache.
    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;
    fs.put(resource, response);

    std::unique_ptr<AsyncRequest> req;
    req = fs.request(resource, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_TRUE(res.notModified);
        EXPECT_FALSE(res.data.get());
        ASSERT_TRUE(bool(res.expires));
        EXPECT_LT(util::now(), *res.expires);
        EXPECT_FALSE(bool(res.modified));
        ASSERT_TRUE(bool(res.etag));
        EXPECT_EQ("snowfall", *res.etag);
        loop.stop();
    });

    loop.run();
}

// Test that we can make a request with etag data that doesn't first try to load
// from cache like a regular request
TEST(DefaultFileSource, TEST_REQUIRES_SERVER(NoCacheRefreshEtagModified)) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    Resource resource { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    resource.priorEtag.emplace("sunshine");

    using namespace std::chrono_literals;

    // Put a fake value into the cache to make sure we're not retrieving anything from the cache.
    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;
    fs.put(resource, response);

    std::unique_ptr<AsyncRequest> req;
    req = fs.request(resource, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_FALSE(res.notModified);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        ASSERT_TRUE(bool(res.etag));
        EXPECT_EQ("snowfall", *res.etag);
        loop.stop();
    });

    loop.run();
}

// Test that we can make a request that doesn't first try to load
// from cache like a regular request.
TEST(DefaultFileSource, TEST_REQUIRES_SERVER(NoCacheFull)) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    Resource resource { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    // Setting any prior field results in skipping the cache.
    resource.priorExpires.emplace(Seconds(0));

    using namespace std::chrono_literals;

    // Put a fake value into the cache to make sure we're not retrieving anything from the cache.
    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;
    fs.put(resource, response);

    std::unique_ptr<AsyncRequest> req;
    req = fs.request(resource, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_FALSE(res.notModified);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        ASSERT_TRUE(bool(res.etag));
        EXPECT_EQ("snowfall", *res.etag);
        loop.stop();
    });

    loop.run();
}

// Test that we can make a request with a Modified field that doesn't first try to load
// from cache like a regular request
TEST(DefaultFileSource, TEST_REQUIRES_SERVER(NoCacheRefreshModifiedNotModified)) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    Resource resource { Resource::Unknown, "http://127.0.0.1:3000/revalidate-modified" };
    resource.priorModified.emplace(Seconds(1420070400)); // January 1, 2015

    using namespace std::chrono_literals;

    // Put a fake value into the cache to make sure we're not retrieving anything from the cache.
    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;
    fs.put(resource, response);

    std::unique_ptr<AsyncRequest> req;
    req = fs.request(resource, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_TRUE(res.notModified);
        EXPECT_FALSE(res.data.get());
        ASSERT_TRUE(bool(res.expires));
        EXPECT_LT(util::now(), *res.expires);
        ASSERT_TRUE(bool(res.modified));
        EXPECT_EQ(Timestamp{ Seconds(1420070400) }, *res.modified);
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

// Test that we can make a request with a Modified field that doesn't first try to load
// from cache like a regular request
TEST(DefaultFileSource, TEST_REQUIRES_SERVER(NoCacheRefreshModifiedModified)) {
    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    Resource resource { Resource::Unknown, "http://127.0.0.1:3000/revalidate-modified" };
    resource.priorModified.emplace(Seconds(1417392000)); // December 1, 2014

    using namespace std::chrono_literals;

    // Put a fake value into the cache to make sure we're not retrieving anything from the cache.
    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;
    fs.put(resource, response);

    std::unique_ptr<AsyncRequest> req;
    req = fs.request(resource, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_FALSE(res.notModified);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_EQ(Timestamp{ Seconds(1420070400) }, *res.modified);
        EXPECT_FALSE(res.etag);
        loop.stop();
    });

    loop.run();
}
