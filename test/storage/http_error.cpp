#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>

#include <cmath>

#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
#define UV_TIMER_PARAMS uv_timer_t*, int
#else
#define UV_TIMER_PARAMS uv_timer_t*
#endif

TEST_F(Storage, HTTPError) {
    SCOPED_TEST(HTTPTemporaryError)
    SCOPED_TEST(HTTPConnectionError)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);

    const auto start = uv_hrtime();

    Request* req1 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/temporary-error" }, uv_default_loop(),
               [&](const Response &res) {
        static int counter = 0;
        switch (counter++) {
        case 0: {
            const auto duration = double(uv_hrtime() - start) / 1e9;
            EXPECT_GT(0.2, duration) << "Initial error request took too long";
            ASSERT_NE(nullptr, res.error);
            EXPECT_EQ(Response::Error::Reason::Server, res.error->reason);
            EXPECT_EQ("HTTP status code 500", res.error->message);
            EXPECT_EQ(false, res.stale);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("", *res.data);
            EXPECT_EQ(0, res.expires);
            EXPECT_EQ(0, res.modified);
            EXPECT_EQ("", res.etag);
        } break;
        case 1: {
            fs.cancel(req1);
            const auto duration = double(uv_hrtime() - start) / 1e9;
            EXPECT_LT(0.99, duration) << "Backoff timer didn't wait 1 second";
            EXPECT_GT(1.2, duration) << "Backoff timer fired too late";
            EXPECT_EQ(nullptr, res.error);
            EXPECT_EQ(false, res.stale);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("Hello World!", *res.data);
            EXPECT_EQ(0, res.expires);
            EXPECT_EQ(0, res.modified);
            EXPECT_EQ("", res.etag);
            HTTPTemporaryError.finish();
        } break;
        }
    });

    Request* req2 = fs.request({ Resource::Unknown, "http://127.0.0.1:3001/" }, uv_default_loop(),
               [&](const Response &res) {
        static int counter = 0;
        static int wait = 0;
        const auto duration = double(uv_hrtime() - start) / 1e9;
        EXPECT_LT(wait - 0.01, duration) << "Backoff timer didn't wait 1 second";
        EXPECT_GT(wait + 0.2, duration) << "Backoff timer fired too late";
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::Connection, res.error->reason);
#ifdef MBGL_HTTP_NSURL
        EXPECT_TRUE(res.error->message ==
                     "The operation couldn’t be completed. (NSURLErrorDomain error -1004.)" ||
                 res.error->message == "Could not connect to the server.")
         << "Full message is: \"" << res.error->message << "\"";
#elif MBGL_HTTP_CURL
        const std::string prefix { "Couldn't connect to server: " };
        EXPECT_STREQ(prefix.c_str(), res.error->message.substr(0, prefix.size()).c_str()) << "Full message is: \"" << res.error->message << "\"";
#else
        FAIL();
#endif
        EXPECT_EQ(false, res.stale);
        ASSERT_FALSE(res.data.get());
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);

        if (counter == 2) {
            fs.cancel(req2);
            HTTPConnectionError.finish();
        }
        wait += (1 << counter);
        counter++;
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
