#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>

TEST_F(Storage, HTTPOtherLoop) {
    SCOPED_TEST(HTTPOtherLoop)

    using namespace mbgl;

    // This file source launches a separate thread to do the processing.
    DefaultFileSource fs(nullptr);

    Request* req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, uv_default_loop(),
               [&](const Response &res) {
        fs.cancel(req);
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        HTTPOtherLoop.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
