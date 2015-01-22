#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>

#include <cmath>

TEST_F(Storage, HTTPHeaderParsing) {
    SCOPED_TEST(HTTPExpiresTest)
    SCOPED_TEST(HTTPCacheControlTest)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test?modified=1420794326&expires=1420797926&etag=foo" }, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data, "Hello World!");
        EXPECT_EQ(res.expires, 1420797926);
        EXPECT_EQ(res.modified, 1420794326);
        EXPECT_EQ(res.etag, "foo");
        EXPECT_EQ(res.message, "");
        HTTPExpiresTest.finish();
    });


    int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::system_clock::now().time_since_epoch()).count();

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test?cachecontrol=max-age=120" }, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data, "Hello World!");
        EXPECT_LT(std::abs(res.expires - now - 120), 2) << "Expiration date isn't about 120 seconds in the future";
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "");
        EXPECT_EQ(res.message, "");
        HTTPCacheControlTest.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
