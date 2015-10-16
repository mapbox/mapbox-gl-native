#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/chrono.hpp>

#include <cmath>

TEST_F(Storage, HTTPHeaderParsing) {
    SCOPED_TEST(HTTPExpiresTest)
    SCOPED_TEST(HTTPCacheControlTest)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);

    Request* req1 = fs.request({ Resource::Unknown,
                 "http://127.0.0.1:3000/test?modified=1420794326&expires=1420797926&etag=foo" },
               uv_default_loop(), [&](const Response &res) {
        fs.cancel(req1);
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(1420797926, res.expires);
        EXPECT_EQ(1420794326, res.modified);
        EXPECT_EQ("foo", res.etag);
        EXPECT_EQ("", res.message);
        HTTPExpiresTest.finish();
    });

    int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                       SystemClock::now().time_since_epoch()).count();

    Request* req2 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test?cachecontrol=max-age=120" },
               uv_default_loop(), [&](const Response &res) {
        fs.cancel(req2);
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_GT(2, std::abs(res.expires - now - 120)) << "Expiration date isn't about 120 seconds in the future";
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);
        HTTPCacheControlTest.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
