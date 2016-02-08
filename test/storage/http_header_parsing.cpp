#include "storage.hpp"

#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

#include <cmath>

TEST_F(Storage, HTTPExpiresParsing) {
    SCOPED_TEST(HTTPExpiresTest)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs(nullptr);

    std::unique_ptr<FileRequest> req1 = fs.request({ Resource::Unknown,
                 "http://127.0.0.1:3000/test?modified=1420794326&expires=1420797926&etag=foo" },
               [&](Response res) {
        req1.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(SystemClock::from_time_t(1420797926), res.expires);
        EXPECT_EQ(SystemClock::from_time_t(1420794326), res.modified);
        EXPECT_EQ("foo", *res.etag);
        loop.stop();
        HTTPExpiresTest.finish();
    });

    loop.run();
}

TEST_F(Storage, HTTPCacheControlParsing) {
    SCOPED_TEST(HTTPCacheControlTest)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs(nullptr);

    std::unique_ptr<FileRequest> req2 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test?cachecontrol=max-age=120" },
               [&](Response res) {
        req2.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_GT(Seconds(2), util::abs(*res.expires - SystemClock::now() - Seconds(120))) << "Expiration date isn't about 120 seconds in the future";
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
        HTTPCacheControlTest.finish();
    });

    loop.run();
}
