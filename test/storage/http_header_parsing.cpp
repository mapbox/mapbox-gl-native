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

    std::unique_ptr<FileRequest> req1 = fs.request({ Resource::Unknown,
                 "http://127.0.0.1:3000/test?modified=1420794326&expires=1420797926&etag=foo" },
               [&](Response res) {
        req1.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(1420797926, res.expires.count());
        EXPECT_EQ(1420794326, res.modified.count());
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

    const Seconds now = toSeconds(SystemClock::now());

    std::unique_ptr<FileRequest> req2 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test?cachecontrol=max-age=120" },
               [&](Response res) {
        req2.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_GT(2, std::abs(res.expires.count() - now.count() - 120)) << "Expiration date isn't about 120 seconds in the future";
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);
        loop.stop();
        HTTPCacheControlTest.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
