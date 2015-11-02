#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>

TEST_F(Storage, HTTPCoalescing) {
    SCOPED_TEST(HTTPCoalescing)

    static int counter = 0;
    const static int total = 4;

    using namespace mbgl;

    DefaultFileSource fs(nullptr);

    static const Response *reference = nullptr;

    const auto complete = [&](const Response &res) {
        counter++;

        // Make sure all of the Response objects are the same.
        if (!reference) {
            reference = &res;
        } else {
            EXPECT_EQ(&res, reference);
        }

        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);

        if (counter >= total) {
            HTTPCoalescing.finish();
        }
    };

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };

    Request* reqs[total];
    for (int i = 0; i < total; i++) {
        reqs[i] = fs.request(resource, uv_default_loop(), [&complete, &fs, &reqs, i] (const Response &res) {
            fs.cancel(reqs[i]);
            complete(res);
        });
    }

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, HTTPMultiple) {
    SCOPED_TEST(HTTPMultiple)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);

    const Response *reference = nullptr;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test?expires=2147483647" };
    Request* req1 = nullptr;
    Request* req2 = nullptr;
    req1 = fs.request(resource, uv_default_loop(), [&] (const Response &res) {
        EXPECT_EQ(nullptr, reference);
        reference = &res;

        // Do not cancel the request right away.
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(2147483647, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);

        // Start a second request for the same resource after the first one has been completed.
        req2 = fs.request(resource, uv_default_loop(), [&] (const Response &res2) {
            // Make sure we get the same object ID as before.
            EXPECT_EQ(reference, &res2);

            // Now cancel both requests after both have been notified.
            fs.cancel(req1);
            fs.cancel(req2);

            EXPECT_EQ(nullptr, res2.error);
            ASSERT_TRUE(res2.data.get());
            EXPECT_EQ("Hello World!", *res2.data);
            EXPECT_EQ(2147483647, res2.expires);
            EXPECT_EQ(0, res2.modified);
            EXPECT_EQ("", res2.etag);

            HTTPMultiple.finish();
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

// Tests that we get stale responses from previous requests when requesting the same thing again.
TEST_F(Storage, HTTPStale) {
    SCOPED_TEST(HTTPStale)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);

    int updates = 0;
    int stale = 0;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };
    Request* req1 = nullptr;
    Request* req2 = nullptr;
    req1 = fs.request(resource, uv_default_loop(), [&] (const Response &res) {
        // Do not cancel the request right away.
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(false, res.stale);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);

        // Don't start the request twice in case this callback gets fired multiple times.
        if (req2) {
            return;
        }

        updates++;

        // Start a second request for the same resource after the first one has been completed.
        req2 = fs.request(resource, uv_default_loop(), [&] (const Response &res2) {
            EXPECT_EQ(nullptr, res2.error);
            ASSERT_TRUE(res2.data.get());
            EXPECT_EQ("Hello World!", *res2.data);
            EXPECT_EQ(0, res2.expires);
            EXPECT_EQ(0, res2.modified);
            EXPECT_EQ("", res2.etag);

            if (res2.stale) {
                EXPECT_EQ(0, stale);
                stale++;
            } else {
                // Now cancel both requests after both have been notified.
                fs.cancel(req1);
                fs.cancel(req2);
                HTTPStale.finish();
            }
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    EXPECT_EQ(1, stale);
    EXPECT_EQ(1, updates);
}
