#include <mbgl/actor/actor.hpp>
#include <mbgl/storage/database_file_source.hpp>
#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/storage/main_resource_loader.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/storage/resource_transform.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

using namespace mbgl;

TEST(MainResourceLoader, TEST_REQUIRES_SERVER(CacheResponse)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    const Resource resource{Resource::Unknown, "http://127.0.0.1:3000/cache"};
    Response response;

    std::unique_ptr<AsyncRequest> req1;
    std::unique_ptr<AsyncRequest> req2;

    req1 = fs.request(resource, [&](Response res) {
        req1.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response 1", *res.data);
        EXPECT_TRUE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
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
            EXPECT_EQ(response.mustRevalidate, res2.mustRevalidate);
            EXPECT_EQ(response.modified, res2.modified);
            EXPECT_EQ(response.etag, res2.etag);

            loop.stop();
        });
    });

    loop.run();
}

TEST(MainResourceLoader, TEST_REQUIRES_SERVER(CacheRevalidateSame)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    const Resource revalidateSame{Resource::Unknown, "http://127.0.0.1:3000/revalidate-same"};
    std::unique_ptr<AsyncRequest> req1;
    std::unique_ptr<AsyncRequest> req2;
    bool gotResponse = false;

    // First request causes the response to get cached.
    req1 = fs.request(revalidateSame, [&](Response res) {
        req1.reset();

        EXPECT_EQ(nullptr, res.error);
        EXPECT_FALSE(res.notModified);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_TRUE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_EQ("snowfall", *res.etag);

        // The first response is stored in the cache, but it has 'must-revalidate' set. This means
        // it can't return the cached response right away and we must wait for the revalidation
        // request to complete. We can distinguish the cached response from the revalided response
        // because the latter has an expiration date, while the cached response doesn't.
        req2 = fs.request(revalidateSame, [&](Response res2) {
            if (!gotResponse) {
                // Even though we could find the response in the database, we send a revalidation
                // request and get a 304 response. Since we haven't sent a reply yet, we're forcing
                // notModified to be false so that implementations can continue to use the
                // notModified flag to skip parsing new data.
                gotResponse = true;
                EXPECT_EQ(nullptr, res2.error);
                EXPECT_FALSE(res2.notModified);
                ASSERT_TRUE(res2.data.get());
                EXPECT_EQ("Response", *res2.data);
                EXPECT_TRUE(bool(res2.expires));
                EXPECT_TRUE(res2.mustRevalidate);
                EXPECT_FALSE(bool(res2.modified));
                EXPECT_EQ("snowfall", *res2.etag);
            } else {
                // The test server sends a Cache-Control header with a max-age of 1 second. This
                // means that our OnlineFileSource implementation will request the tile again after
                // 1 second. This time, our request already had a prior response, so we don't need
                // to send the data again, and instead can actually forward the notModified flag.
                req2.reset();
                EXPECT_EQ(nullptr, res2.error);
                EXPECT_TRUE(res2.notModified);
                EXPECT_FALSE(res2.data.get());
                EXPECT_TRUE(bool(res2.expires));
                EXPECT_TRUE(res2.mustRevalidate);
                EXPECT_FALSE(bool(res2.modified));
                EXPECT_EQ("snowfall", *res2.etag);
                loop.stop();
            }
        });
    });

    loop.run();
}

TEST(MainResourceLoader, TEST_REQUIRES_SERVER(CacheRevalidateModified)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    const Resource revalidateModified{Resource::Unknown, "http://127.0.0.1:3000/revalidate-modified"};
    std::unique_ptr<AsyncRequest> req1;
    std::unique_ptr<AsyncRequest> req2;
    bool gotResponse = false;

    // First request causes the response to get cached.
    req1 = fs.request(revalidateModified, [&](Response res) {
        req1.reset();

        EXPECT_EQ(nullptr, res.error);
        EXPECT_FALSE(res.notModified);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_TRUE(res.mustRevalidate);
        EXPECT_EQ(Timestamp{Seconds(1420070400)}, *res.modified);
        EXPECT_FALSE(res.etag);

        // The first response is stored in the cache, but it has 'must-revalidate' set. This means
        // it can't return the cached response right away and we must wait for the revalidation
        // request to complete. We can distinguish the cached response from the revalided response
        // because the latter has an expiration date, while the cached response doesn't.
        req2 = fs.request(revalidateModified, [&, res](Response res2) {
            if (!gotResponse) {
                // Even though we could find the response in the database, we send a revalidation
                // request and get a 304 response. Since we haven't sent a reply yet, we're forcing
                // notModified to be false so that implementations can continue to use the
                // notModified flag to skip parsing new data.
                gotResponse = true;
                EXPECT_EQ(nullptr, res2.error);
                EXPECT_FALSE(res2.notModified);
                ASSERT_TRUE(res2.data.get());
                EXPECT_EQ("Response", *res2.data);
                EXPECT_TRUE(bool(res2.expires));
                EXPECT_TRUE(res2.mustRevalidate);
                EXPECT_EQ(Timestamp{Seconds(1420070400)}, *res2.modified);
                EXPECT_FALSE(res2.etag);
            } else {
                // The test server sends a Cache-Control header with a max-age of 1 second. This
                // means that our OnlineFileSource implementation will request the tile again after
                // 1 second. This time, our request already had a prior response, so we don't need
                // to send the data again, and instead can actually forward the notModified flag.
                req2.reset();
                EXPECT_EQ(nullptr, res2.error);
                EXPECT_TRUE(res2.notModified);
                EXPECT_FALSE(res2.data.get());
                EXPECT_TRUE(bool(res2.expires));
                EXPECT_TRUE(res2.mustRevalidate);
                EXPECT_EQ(Timestamp{Seconds(1420070400)}, *res2.modified);
                EXPECT_FALSE(res2.etag);
                loop.stop();
            }
        });
    });

    loop.run();
}

TEST(MainResourceLoader, TEST_REQUIRES_SERVER(CacheRevalidateEtag)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    const Resource revalidateEtag{Resource::Unknown, "http://127.0.0.1:3000/revalidate-etag"};
    std::unique_ptr<AsyncRequest> req1;
    std::unique_ptr<AsyncRequest> req2;

    // First request causes the response to get cached.
    req1 = fs.request(revalidateEtag, [&](Response res) {
        req1.reset();

        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response 1", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_TRUE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_EQ("response-1", *res.etag);

        // Second request does not return the cached response, since it had Cache-Control: must-revalidate set.
        req2 = fs.request(revalidateEtag, [&, res](Response res2) {
            req2.reset();

            EXPECT_EQ(nullptr, res2.error);
            ASSERT_TRUE(res2.data.get());
            EXPECT_NE(res.data, res2.data);
            EXPECT_EQ("Response 2", *res2.data);
            EXPECT_FALSE(bool(res2.expires));
            EXPECT_TRUE(res2.mustRevalidate);
            EXPECT_FALSE(bool(res2.modified));
            EXPECT_EQ("response-2", *res2.etag);

            loop.stop();
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
TEST(MainResourceLoader, TEST_REQUIRES_SERVER(HTTPIssue1369)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    const Resource resource{Resource::Unknown, "http://127.0.0.1:3000/test"};

    auto req = fs.request(resource, [&](Response) { ADD_FAILURE() << "Callback should not be called"; });
    req.reset();
    req = fs.request(resource, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

TEST(MainResourceLoader, OptionalNonExpired) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    const Resource optionalResource{
        Resource::Unknown, "http://127.0.0.1:3000/test", {}, Resource::LoadingMethod::CacheOnly};

    using namespace std::chrono_literals;

    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;

    std::unique_ptr<AsyncRequest> req;
    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    dbfs->forward(optionalResource, response, [&] {
        req = fs.request(optionalResource, [&](Response res) {
            req.reset();
            EXPECT_EQ(nullptr, res.error);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("Cached value", *res.data);
            ASSERT_TRUE(bool(res.expires));
            EXPECT_EQ(*response.expires, *res.expires);
            EXPECT_FALSE(res.mustRevalidate);
            EXPECT_FALSE(bool(res.modified));
            EXPECT_FALSE(bool(res.etag));
            loop.stop();
        });
    });

    loop.run();
}

TEST(MainResourceLoader, OptionalExpired) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    const Resource optionalResource{
        Resource::Unknown, "http://127.0.0.1:3000/test", {}, Resource::LoadingMethod::CacheOnly};

    using namespace std::chrono_literals;

    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() - 1h;
    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    std::unique_ptr<AsyncRequest> req;
    dbfs->forward(optionalResource, response, [&] {
        req = fs.request(optionalResource, [&](Response res) {
            req.reset();
            EXPECT_EQ(nullptr, res.error);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("Cached value", *res.data);
            ASSERT_TRUE(bool(res.expires));
            EXPECT_EQ(*response.expires, *res.expires);
            EXPECT_FALSE(res.mustRevalidate);
            EXPECT_FALSE(bool(res.modified));
            EXPECT_FALSE(bool(res.etag));
            loop.stop();
        });
    });

    loop.run();
}

TEST(MainResourceLoader, OptionalNotFound) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    const Resource optionalResource{
        Resource::Unknown, "http://127.0.0.1:3000/test", {}, Resource::LoadingMethod::CacheOnly};

    using namespace std::chrono_literals;

    std::unique_ptr<AsyncRequest> req;
    req = fs.request(optionalResource, [&](Response res) {
        req.reset();
        ASSERT_TRUE(res.error.get());
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        EXPECT_EQ("Not found in offline database", res.error->message);
        EXPECT_FALSE(res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

// Test that a network only request doesn't attempt to load data from the cache.
TEST(MainResourceLoader, TEST_REQUIRES_SERVER(NoCacheRefreshEtagNotModified)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    Resource resource { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    resource.loadingMethod = Resource::LoadingMethod::NetworkOnly;
    resource.priorEtag.emplace("snowfall");

    using namespace std::chrono_literals;

    // Put a fake value into the cache to make sure we're not retrieving anything from the cache.
    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;
    std::unique_ptr<AsyncRequest> req;
    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    dbfs->forward(resource, response, [&] {
        req = fs.request(resource, [&](Response res) {
            req.reset();
            EXPECT_EQ(nullptr, res.error);
            EXPECT_TRUE(res.notModified);
            EXPECT_FALSE(res.data.get());
            ASSERT_TRUE(bool(res.expires));
            EXPECT_LT(util::now(), *res.expires);
            EXPECT_TRUE(res.mustRevalidate);
            EXPECT_FALSE(bool(res.modified));
            ASSERT_TRUE(bool(res.etag));
            EXPECT_EQ("snowfall", *res.etag);
            loop.stop();
        });
    });

    loop.run();
}

// Test that a network only request doesn't attempt to load data from the cache.
TEST(MainResourceLoader, TEST_REQUIRES_SERVER(NoCacheRefreshEtagModified)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    Resource resource { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    resource.loadingMethod = Resource::LoadingMethod::NetworkOnly;
    resource.priorEtag.emplace("sunshine");

    using namespace std::chrono_literals;

    // Put a fake value into the cache to make sure we're not retrieving anything from the cache.
    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;
    std::unique_ptr<AsyncRequest> req;
    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    dbfs->forward(resource, response, [&] {
        req = fs.request(resource, [&](Response res) {
            req.reset();
            EXPECT_EQ(nullptr, res.error);
            EXPECT_FALSE(res.notModified);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("Response", *res.data);
            EXPECT_FALSE(bool(res.expires));
            EXPECT_TRUE(res.mustRevalidate);
            EXPECT_FALSE(bool(res.modified));
            ASSERT_TRUE(bool(res.etag));
            EXPECT_EQ("snowfall", *res.etag);
            loop.stop();
        });
    });

    loop.run();
}

// Test that a network only request doesn't attempt to load data from the cache.
TEST(MainResourceLoader, TEST_REQUIRES_SERVER(NoCacheFull)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    Resource resource { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    resource.loadingMethod = Resource::LoadingMethod::NetworkOnly;

    using namespace std::chrono_literals;

    // Put a fake value into the cache to make sure we're not retrieving anything from the cache.
    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;
    std::unique_ptr<AsyncRequest> req;
    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    dbfs->forward(resource, response, [&] {
        req = fs.request(resource, [&](Response res) {
            req.reset();
            EXPECT_EQ(nullptr, res.error);
            EXPECT_FALSE(res.notModified);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("Response", *res.data);
            EXPECT_FALSE(bool(res.expires));
            EXPECT_TRUE(res.mustRevalidate);
            EXPECT_FALSE(bool(res.modified));
            ASSERT_TRUE(bool(res.etag));
            EXPECT_EQ("snowfall", *res.etag);
            loop.stop();
        });
    });

    loop.run();
}

// Test that we can make a request with a Modified field that doesn't first try to load
// from cache like a regular request
TEST(MainResourceLoader, TEST_REQUIRES_SERVER(NoCacheRefreshModifiedNotModified)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    Resource resource { Resource::Unknown, "http://127.0.0.1:3000/revalidate-modified" };
    resource.loadingMethod = Resource::LoadingMethod::NetworkOnly;
    resource.priorModified.emplace(Seconds(1420070400)); // January 1, 2015

    using namespace std::chrono_literals;

    // Put a fake value into the cache to make sure we're not retrieving anything from the cache.
    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;
    std::unique_ptr<AsyncRequest> req;
    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    dbfs->forward(resource, response, [&] {
        req = fs.request(resource, [&](Response res) {
            req.reset();
            EXPECT_EQ(nullptr, res.error);
            EXPECT_TRUE(res.notModified);
            EXPECT_FALSE(res.data.get());
            ASSERT_TRUE(bool(res.expires));
            EXPECT_LT(util::now(), *res.expires);
            EXPECT_TRUE(res.mustRevalidate);
            ASSERT_TRUE(bool(res.modified));
            EXPECT_EQ(Timestamp{Seconds(1420070400)}, *res.modified);
            EXPECT_FALSE(bool(res.etag));
            loop.stop();
        });
    });

    loop.run();
}

// Test that we can make a request with a Modified field that doesn't first try to load
// from cache like a regular request
TEST(MainResourceLoader, TEST_REQUIRES_SERVER(NoCacheRefreshModifiedModified)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    Resource resource { Resource::Unknown, "http://127.0.0.1:3000/revalidate-modified" };
    resource.loadingMethod = Resource::LoadingMethod::NetworkOnly;
    resource.priorModified.emplace(Seconds(1417392000)); // December 1, 2014

    using namespace std::chrono_literals;

    // Put a fake value into the cache to make sure we're not retrieving anything from the cache.
    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.expires = util::now() + 1h;
    std::unique_ptr<AsyncRequest> req;
    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    dbfs->forward(resource, response, [&] {
        req = fs.request(resource, [&](Response res) {
            req.reset();
            EXPECT_EQ(nullptr, res.error);
            EXPECT_FALSE(res.notModified);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("Response", *res.data);
            EXPECT_FALSE(bool(res.expires));
            EXPECT_TRUE(res.mustRevalidate);
            EXPECT_EQ(Timestamp{Seconds(1420070400)}, *res.modified);
            EXPECT_FALSE(res.etag);
            loop.stop();
        });
    });

    loop.run();
}

TEST(MainResourceLoader, TEST_REQUIRES_SERVER(SetResourceTransform)) {
    util::RunLoop loop;
    MainResourceLoader resourceLoader(ResourceOptions{});
    std::shared_ptr<FileSource> fs =
        FileSourceManager::get()->getFileSource(FileSourceType::Network, ResourceOptions{});
    auto onlinefs = std::static_pointer_cast<OnlineFileSource>(fs);

    // Translates the URL "localhost://test to http://127.0.0.1:3000/test
    Actor<ResourceTransform::TransformCallback> transform(
        loop, [](Resource::Kind, const std::string& url, ResourceTransform::FinishedCallback cb) {
            if (url == "localhost://test") {
                cb("http://127.0.0.1:3000/test");
            } else {
                cb(url);
            }
        });

    onlinefs->setResourceTransform(
        {[actorRef = transform.self()](
             Resource::Kind kind, const std::string& url, ResourceTransform::FinishedCallback cb) {
            actorRef.invoke(&ResourceTransform::TransformCallback::operator(), kind, url, std::move(cb));
        }});
    const Resource resource1{Resource::Unknown, "localhost://test"};

    std::unique_ptr<AsyncRequest> req;
    req = resourceLoader.request(resource1, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();

    onlinefs->setResourceTransform({});
    const Resource resource2{Resource::Unknown, "http://127.0.0.1:3000/test"};

    req = resourceLoader.request(resource2, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

TEST(MainResourceLoader, SetResourceCachePath) {
    util::RunLoop loop;
    MainResourceLoader resourceLoader(ResourceOptions{});
    std::shared_ptr<FileSource> fs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    auto dbfs = std::static_pointer_cast<DatabaseFileSource>(fs);
    dbfs->setDatabasePath("./new_offline.db", [&loop] { loop.stop(); });
    loop.run();
}

// Test that a stale cache file that has must-revalidate set will trigger a response.
TEST(MainResourceLoader, TEST_REQUIRES_SERVER(RespondToStaleMustRevalidate)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    Resource resource { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    resource.loadingMethod = Resource::LoadingMethod::CacheOnly;

    // using namespace std::chrono_literals;

    // Put an existing value in the cache that has expired, and has must-revalidate set.
    Response response;
    response.data = std::make_shared<std::string>("Cached value");
    response.modified = Timestamp(Seconds(1417392000)); // December 1, 2014
    response.expires = Timestamp(Seconds(1417392000));
    response.mustRevalidate = true;
    response.etag.emplace("snowfall");
    std::unique_ptr<AsyncRequest> req;
    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    dbfs->forward(resource, response, [&] {
        req = fs.request(resource, [&](Response res) {
            req.reset();
            ASSERT_TRUE(res.error.get());
            EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
            EXPECT_EQ("Cached resource is unusable", res.error->message);
            EXPECT_FALSE(res.notModified);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("Cached value", *res.data);
            ASSERT_TRUE(res.expires);
            EXPECT_EQ(Timestamp{Seconds(1417392000)}, *res.expires);
            EXPECT_TRUE(res.mustRevalidate);
            ASSERT_TRUE(res.modified);
            EXPECT_EQ(Timestamp{Seconds(1417392000)}, *res.modified);
            ASSERT_TRUE(res.etag);
            EXPECT_EQ("snowfall", *res.etag);

            resource.priorEtag = res.etag;
            resource.priorModified = res.modified;
            resource.priorExpires = res.expires;
            resource.priorData = res.data;

            loop.stop();
        });
    });

    loop.run();

    // Now run this request again, with the data we gathered from the previous stale/unusable
    // request. We're replacing the data so that we can check that the MainResourceLoader doesn't
    // attempt another database access if we already have the value.
    resource.loadingMethod = Resource::LoadingMethod::NetworkOnly;
    resource.priorData = std::make_shared<std::string>("Prior value");

    req = fs.request(resource, [&](Response res) {
        req.reset();
        ASSERT_EQ(nullptr, res.error.get());
        // Since the data was found in the cache, we're doing a revalidation request. Yet, since
        // this request hasn't returned data before, we're setting notModified to false in the
        // OnlineFileSource to ensure that requestors know that this is the first time they're
        // seeing this data.
        EXPECT_FALSE(res.notModified);
        ASSERT_TRUE(res.data.get());
        // Ensure that it's the value that we manually inserted into the cache rather than the value
        // the server returns, since we should be executing a revalidation request which doesn't
        // return new data, only a 304 Not Modified response.
        EXPECT_EQ("Prior value", *res.data);
        ASSERT_TRUE(res.expires);
        EXPECT_LE(util::now(), *res.expires);
        EXPECT_TRUE(res.mustRevalidate);
        ASSERT_TRUE(res.modified);
        EXPECT_EQ(Timestamp{ Seconds(1417392000) }, *res.modified);
        ASSERT_TRUE(res.etag);
        EXPECT_EQ("snowfall", *res.etag);
        loop.stop();
    });

    loop.run();
}

// Test that requests for expired resources have lower priority than requests for new resources
TEST(MainResourceLoader, TEST_REQUIRES_SERVER(CachedResourceLowPriority)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    Response response;
    std::size_t online_response_counter = 0;

    using namespace std::chrono_literals;
    response.expires = util::now() - 1h;

    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    std::shared_ptr<FileSource> onlineFs =
        FileSourceManager::get()->getFileSource(FileSourceType::Network, ResourceOptions{});

    // Put existing values into the cache.
    Resource resource1{Resource::Unknown, "http://127.0.0.1:3000/load/3", {}, Resource::LoadingMethod::All};
    response.data = std::make_shared<std::string>("Cached Request 3");
    dbfs->forward(resource1, response);

    Resource resource2{Resource::Unknown, "http://127.0.0.1:3000/load/4", {}, Resource::LoadingMethod::All};
    response.data = std::make_shared<std::string>("Cached Request 4");
    dbfs->forward(resource2, response);

    onlineFs->setProperty(MAX_CONCURRENT_REQUESTS_KEY, 1u);
    fs.pause();
    NetworkStatus::Set(NetworkStatus::Status::Offline);

    // Ensure that the online requests for new resources are processed first.
    Resource nonCached0{Resource::Unknown, "http://127.0.0.1:3000/load/0", {}, Resource::LoadingMethod::All};
    std::unique_ptr<AsyncRequest> req0 = fs.request(nonCached0, [&](Response res) {
        req0.reset();
        EXPECT_EQ(online_response_counter, 0); // make sure this is responded first
        EXPECT_EQ("Request 0", *res.data);
    });

    Resource nonCached1{Resource::Unknown, "http://127.0.0.1:3000/load/1", {}, Resource::LoadingMethod::All};
    std::unique_ptr<AsyncRequest> req1 = fs.request(nonCached1, [&](Response res) {
        online_response_counter++;
        req1.reset();
        EXPECT_EQ(online_response_counter, 1); // make sure this is responded second
        EXPECT_EQ("Request 1", *res.data);
    });

    bool req3CachedResponseReceived = false;
    std::unique_ptr<AsyncRequest> req3 = fs.request(resource1, [&](Response res) {
        // Offline callback is received first
        if (!req3CachedResponseReceived) {
            EXPECT_EQ("Cached Request 3", *res.data);
            req3CachedResponseReceived = true;
        } else {
            online_response_counter++;
            req3.reset();
            EXPECT_EQ(online_response_counter, 3);
            EXPECT_EQ("Request 3", *res.data);
        }
    });

    bool req4CachedResponseReceived = false;
    std::unique_ptr<AsyncRequest> req4 = fs.request(resource2, [&](Response res) {
        // Offline callback is received first
        if (!req4CachedResponseReceived) {
            EXPECT_EQ("Cached Request 4", *res.data);
            req4CachedResponseReceived = true;
        } else {
            online_response_counter++;
            req4.reset();
            EXPECT_EQ(online_response_counter, 4);
            EXPECT_EQ("Request 4", *res.data);
            loop.stop();
        }
    });

    Resource nonCached2{Resource::Unknown, "http://127.0.0.1:3000/load/2", {}, Resource::LoadingMethod::All};
    std::unique_ptr<AsyncRequest> req2 = fs.request(nonCached2, [&](Response res) {
        online_response_counter++;
        req2.reset();
        EXPECT_EQ(online_response_counter, 2); // make sure this is responded third
        EXPECT_EQ("Request 2", *res.data);
    });

    fs.resume();
    NetworkStatus::Set(NetworkStatus::Status::Online);

    loop.run();
}

TEST(MainResourceLoader, TEST_REQUIRES_SERVER(NoDoubleDispatch)) {
    util::RunLoop loop;
    MainResourceLoader fs(ResourceOptions{});

    const Resource resource{Resource::Unknown, "http://127.0.0.1:3000/revalidate-same"};
    Response response;
    response.data = std::make_shared<std::string>("data");
    response.etag.emplace("snowfall");

    std::unique_ptr<AsyncRequest> req;
    unsigned responseCount = 0u;
    std::shared_ptr<FileSource> dbfs =
        FileSourceManager::get()->getFileSource(FileSourceType::Database, ResourceOptions{});
    dbfs->forward(resource, response, [&] {
        req = fs.request(resource, [&](Response res) {
            EXPECT_EQ(nullptr, res.error);
            EXPECT_FALSE(bool(res.modified));
            EXPECT_TRUE(bool(res.etag));
            EXPECT_EQ("snowfall", *res.etag);
            if (!res.notModified) {
                ASSERT_TRUE(res.data.get());
                EXPECT_EQ("data", *res.data);
                ++responseCount;
            }
        });
    });

    util::Timer timer;
    timer.start(Milliseconds(100), Duration::zero(), [&loop, &responseCount] {
        EXPECT_EQ(1u, responseCount);
        loop.stop();
    });

    loop.run();
}
