#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>

TEST_F(Storage, HTTPOtherLoop) {
    SCOPED_TEST(HTTPOtherLoop)

    using namespace mbgl;

    // This file source launches a separate thread to do the processing.
    DefaultFileSource fs(nullptr);

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data, "Hello World!");
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "");
        EXPECT_EQ(res.message, "");
        HTTPOtherLoop.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
