#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/chrono.hpp>

#include <cmath>

TEST_F(Storage, HTTPHeaderParsing) {
    SCOPED_TEST(HTTPExpiresTest)
    SCOPED_TEST(HTTPCacheControlTest)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    auto &env = *static_cast<const Environment *>(nullptr);

    fs.request({ Resource::Unknown,
                 "http://127.0.0.1:3000/test?modified=1420794326&expires=1420797926&etag=foo" },
               uv_default_loop(), env, [&](const Response &res) {
        EXPECT_EQ(Response::Successful, res.status);
        ASSERT_TRUE(!!res.data);
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(1420797926, res.expires);
        EXPECT_EQ(1420794326, res.modified);
        EXPECT_EQ("foo", res.etag);
        EXPECT_EQ("", res.message);
        HTTPExpiresTest.finish();
    });

    int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                       SystemClock::now().time_since_epoch()).count();

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test?cachecontrol=max-age=120" },
               uv_default_loop(), env, [&](const Response &res) {
        EXPECT_EQ(Response::Successful, res.status);
        ASSERT_TRUE(!!res.data);
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_GT(2, std::abs(res.expires - now - 120)) << "Expiration date isn't about 120 seconds in the future";
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);
        HTTPCacheControlTest.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
