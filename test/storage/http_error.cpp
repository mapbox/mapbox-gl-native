#include "../util.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>

#include <cmath>

TEST(Storage, HTTPError) {
    SCOPED_TEST(HTTPTemporaryError)
    SCOPED_TEST(HTTPConnectionError)

    using namespace mbgl;

    uv_timer_t statusChange;
    uv_timer_init(uv_default_loop(), &statusChange);
    uv_timer_start(&statusChange, [](uv_timer_t *, int) {
        NetworkStatus::Reachable();
    }, 500, 500);
    uv_unref((uv_handle_t *)&statusChange);

    DefaultFileSource fs(nullptr, uv_default_loop());

    auto start = uv_hrtime();

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/temporary-error" }, uv_default_loop(), [&](const Response &res) {
        const auto duration = double(uv_hrtime() - start) / 1e9;
        EXPECT_GT(duration, 1) << "Backoff timer didn't wait 1 second";
        EXPECT_LT(duration, 1.2) << "Backoff timer fired too late";
        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data, "Hello World!");
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "");
        EXPECT_EQ(res.message, "");

        HTTPTemporaryError.finish();
    });

    fs.request({ Resource::Unknown, "http://127.0.0.1:3001/" }, uv_default_loop(), [&](const Response &res) {
        const auto duration = double(uv_hrtime() - start) / 1e9;
        // 1.5 seconds == 4 retries, with a 500ms timeout (see above).
        EXPECT_GT(duration, 1.5) << "Resource wasn't retried the correct number of times";
        EXPECT_LT(duration, 1.7) << "Resource wasn't retried the correct number of times";
        EXPECT_EQ(res.status, Response::Error);
        EXPECT_TRUE(res.message == "Couldn't connect to server" || res.message == "The operation couldn’t be completed. (NSURLErrorDomain error -1004.)");
        EXPECT_EQ(res.data, "");
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "");
        HTTPConnectionError.finish();
    });


    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    uv_timer_stop(&statusChange);
    uv_close(reinterpret_cast<uv_handle_t *>(&statusChange), nullptr);

    // Run again so that the timer handle can be properly closed.
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
