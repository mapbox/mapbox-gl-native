#include "storage.hpp"

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/run_loop.hpp>

class DefaultFileSourceTest : public Storage {};

TEST_F(DefaultFileSourceTest, TEST_REQUIRES_SERVER(CacheResponse)) {
    SCOPED_TEST(CacheResponse);

    using namespace mbgl;

    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/cache" };
    Response response;

    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<FileRequest> req2;

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
            CacheResponse.finish();
        });
    });

    loop.run();
}

TEST_F(DefaultFileSourceTest, TEST_REQUIRES_SERVER(CacheRevalidateSame)) {
    SCOPED_TEST(CacheRevalidateSame)

    using namespace mbgl;

    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource revalidateSame { Resource::Unknown, "http://127.0.0.1:3000/revalidate-same" };
    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<FileRequest> req2;
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
                CacheRevalidateSame.finish();
            }
        });
    });

    loop.run();
}

TEST_F(DefaultFileSourceTest, TEST_REQUIRES_SERVER(CacheRevalidateModified)) {
    SCOPED_TEST(CacheRevalidateModified)

    using namespace mbgl;

    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource revalidateModified{ Resource::Unknown,
                                       "http://127.0.0.1:3000/revalidate-modified" };
    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<FileRequest> req2;
    uint16_t counter = 0;

    // First request causes the response to get cached.
    req1 = fs.request(revalidateModified, [&](Response res) {
        req1.reset();

        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Response", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_EQ(SystemClock::from_time_t(1420070400), *res.modified);
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
                EXPECT_EQ(SystemClock::from_time_t(1420070400), *res2.modified);
                EXPECT_FALSE(res2.etag);

                loop.stop();
                CacheRevalidateModified.finish();
            }
        });
    });

    loop.run();
}

TEST_F(DefaultFileSourceTest, TEST_REQUIRES_SERVER(CacheRevalidateEtag)) {
    SCOPED_TEST(CacheRevalidateEtag)

    using namespace mbgl;

    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    const Resource revalidateEtag { Resource::Unknown, "http://127.0.0.1:3000/revalidate-etag" };
    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<FileRequest> req2;
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
                CacheRevalidateEtag.finish();
            }
        });
    });

    loop.run();
}
