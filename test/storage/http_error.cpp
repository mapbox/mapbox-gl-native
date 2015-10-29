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

    uv_timer_t statusChange;
    uv_timer_init(uv_default_loop(), &statusChange);
    uv_timer_start(&statusChange, [](UV_TIMER_PARAMS) {
        NetworkStatus::Reachable();
    }, 500, 500);
    uv_unref((uv_handle_t *)&statusChange);

    DefaultFileSource fs(nullptr);

    auto start = uv_hrtime();

    Request* req1 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/temporary-error" }, uv_default_loop(),
               [&](const Response &res) {
        fs.cancel(req1);
        const auto duration = double(uv_hrtime() - start) / 1e9;
        EXPECT_LT(1, duration) << "Backoff timer didn't wait 1 second";
        EXPECT_GT(1.2, duration) << "Backoff timer fired too late";
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);

        HTTPTemporaryError.finish();
    });

    Request* req2 = fs.request({ Resource::Unknown, "http://127.0.0.1:3001/" }, uv_default_loop(),
               [&](const Response &res) {
        fs.cancel(req2);
        const auto duration = double(uv_hrtime() - start) / 1e9;
        // 1.5 seconds == 4 retries, with a 500ms timeout (see above).
        EXPECT_LT(1.4, duration) << "Resource wasn't retried the correct number of times";
        EXPECT_GT(1.7, duration) << "Resource wasn't retried the correct number of times";
        EXPECT_EQ(Response::Error, res.status);
        EXPECT_EQ(false, res.stale);
#ifdef MBGL_HTTP_NSURL
        EXPECT_TRUE(res.message ==
                        "The operation couldn’t be completed. (NSURLErrorDomain error -1004.)" ||
                    res.message == "Could not connect to the server.")
            << "Full message is: \"" << res.message << "\"";
#elif MBGL_HTTP_CURL
        const std::string prefix { "Couldn't connect to server: " };
        EXPECT_STREQ(prefix.c_str(), res.message.substr(0, prefix.size()).c_str()) << "Full message is: \"" << res.message << "\"";
#else
        FAIL();
#endif
        ASSERT_FALSE(res.data.get());
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        HTTPConnectionError.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    uv_timer_stop(&statusChange);
    uv_close(reinterpret_cast<uv_handle_t *>(&statusChange), nullptr);

    // Run again so that the timer handle can be properly closed.
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
