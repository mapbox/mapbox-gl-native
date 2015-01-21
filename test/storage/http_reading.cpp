#include "../util.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>

TEST(Storage, HTTPReading) {
    SCOPED_TEST(HTTPTest)
    SCOPED_TEST(HTTP404)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    const auto mainThread = uv_thread_self();

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(mainThread, uv_thread_self());
        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data, "Hello World!");
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "");
        EXPECT_EQ(res.message, "");
        HTTPTest.finish();
    });

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/doesnotexist" }, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(mainThread, uv_thread_self());
        EXPECT_EQ(res.status, Response::Error);
        EXPECT_EQ(res.message, "HTTP status code 404");
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "");
        HTTP404.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
