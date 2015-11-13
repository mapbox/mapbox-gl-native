#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

#include <cmath>

TEST_F(Storage, HTTPExpiresParsing) {
    SCOPED_TEST(HTTPExpiresTest)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);
    util::RunLoop loop(uv_default_loop());

    Request* req1 = fs.request({ Resource::Unknown,
                 "http://127.0.0.1:3000/test?modified=1420794326&expires=1420797926&etag=foo" },
               [&](Response res) {
        fs.cancel(req1);
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(1420797926, res.expires);
        EXPECT_EQ(1420794326, res.modified);
        EXPECT_EQ("foo", res.etag);
        loop.stop();
        HTTPExpiresTest.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, HTTPCacheControlParsing) {
    SCOPED_TEST(HTTPCacheControlTest)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);
    util::RunLoop loop(uv_default_loop());

    int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                       SystemClock::now().time_since_epoch()).count();

    Request* req2 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test?cachecontrol=max-age=120" },
               [&](Response res) {
        fs.cancel(req2);
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_GT(2, std::abs(res.expires - now - 120)) << "Expiration date isn't about 120 seconds in the future";
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        loop.stop();
        HTTPCacheControlTest.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
