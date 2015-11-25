#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, HTTPCoalescing) {
    SCOPED_TEST(HTTPCoalescing)

    static int counter = 0;
    const static int total = 4;

    using namespace mbgl;

    DefaultFileSource fs(nullptr);
    util::RunLoop loop(uv_default_loop());

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
        EXPECT_EQ(Seconds::zero(), res.expires);
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);

        if (counter >= total) {
            loop.stop();
            HTTPCoalescing.finish();
        }
    };

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };

    std::unique_ptr<FileRequest> reqs[total];
    for (int i = 0; i < total; i++) {
        reqs[i] = fs.request(resource, [&complete, &fs, &reqs, i] (Response res) {
            reqs[i].reset();
            complete(res);
        });
    }

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, HTTPMultiple) {
    SCOPED_TEST(HTTPMultiple)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);
    util::RunLoop loop(uv_default_loop());

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test?expires=2147483647" };
    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<FileRequest> req2;
    req1 = fs.request(resource, [&] (Response res) {
        // Do not cancel the request right away.
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(2147483647, res.expires.count());
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);

        // Start a second request for the same resource after the first one has been completed.
        req2 = fs.request(resource, [&, res] (Response res2) {
            // Make sure we get the same data as before.
            EXPECT_EQ(res.data.get(), res2.data.get());

            // Now cancel both requests after both have been notified.
            req1.reset();
            req2.reset();

            EXPECT_EQ(nullptr, res2.error);
            ASSERT_TRUE(res2.data.get());
            EXPECT_EQ("Hello World!", *res2.data);
            EXPECT_EQ(2147483647, res2.expires.count());
            EXPECT_EQ(Seconds::zero(), res2.modified);
            EXPECT_EQ("", res2.etag);

            loop.stop();
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
    util::RunLoop loop(uv_default_loop());

    int updates = 0;
    int stale = 0;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };
    std::unique_ptr<FileRequest> req1;
    std::unique_ptr<FileRequest> req2;
    req1 = fs.request(resource, [&] (Response res) {
        // Do not cancel the request right away.
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(false, res.stale);
        EXPECT_EQ(Seconds::zero(), res.expires);
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);

        // Don't start the request twice in case this callback gets fired multiple times.
        if (req2) {
            return;
        }

        updates++;

        // Start a second request for the same resource after the first one has been completed.
        req2 = fs.request(resource, [&] (Response res2) {
            EXPECT_EQ(nullptr, res2.error);
            ASSERT_TRUE(res2.data.get());
            EXPECT_EQ("Hello World!", *res2.data);
            EXPECT_EQ(Seconds::zero(), res2.expires);
            EXPECT_EQ(Seconds::zero(), res2.modified);
            EXPECT_EQ("", res2.etag);

            if (res2.stale) {
                EXPECT_EQ(0, stale);
                stale++;
            } else {
                // Now cancel both requests after both have been notified.
                req1.reset();
                req2.reset();
                loop.stop();
                HTTPStale.finish();
            }
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    EXPECT_EQ(1, stale);
    EXPECT_EQ(1, updates);
}
