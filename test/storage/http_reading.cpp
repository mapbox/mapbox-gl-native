#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>

TEST_F(Storage, HTTPReading) {
    SCOPED_TEST(HTTPTest)
    SCOPED_TEST(HTTP404)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    const auto mainThread = uv_thread_self();

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(uv_thread_self(), mainThread);
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ("Hello World!", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);
        HTTPTest.finish();
    });

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/doesnotexist" }, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(uv_thread_self(), mainThread);
        EXPECT_EQ(Response::Error, res.status);
        EXPECT_EQ("HTTP status code 404", res.message);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        HTTP404.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
