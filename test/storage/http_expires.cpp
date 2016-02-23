#include "storage.hpp"

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

TEST_F(Storage, HTTPRetryDelayOnExpiredTile) {
    SCOPED_TEST(HTTPRetryDelayOnExpiredTile)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    int counter = 0;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test?expires=10000" };
    std::unique_ptr<FileRequest> req = fs.request(resource, [&](Response res) {
        counter++;
        EXPECT_EQ(nullptr, res.error);
        EXPECT_GT(SystemClock::now(), res.expires);
    });

    util::Timer timer;
    timer.start(Milliseconds(500), Duration::zero(), [&] () {
        loop.stop();
    });

    loop.run();

    EXPECT_EQ(1, counter);

    HTTPRetryDelayOnExpiredTile.finish();
}

TEST_F(Storage, HTTPRetryOnClockSkew) {
    SCOPED_TEST(HTTPRetryOnClockSkew)

    using namespace mbgl;

    util::RunLoop loop;
    DefaultFileSource fs(":memory:", ".");

    int counter = 0;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/clockskew" };
    std::unique_ptr<FileRequest> req1 = fs.request(resource, [&](Response res) {
        switch (counter++) {
        case 0: {
            EXPECT_EQ(nullptr, res.error);
            EXPECT_GT(SystemClock::now(), res.expires);
        } break;
        case 1: {
            EXPECT_EQ(nullptr, res.error);

            auto now = SystemClock::now();
            EXPECT_LT(now + Seconds(40), res.expires) << "Expiration not interpolated to 60s";
            EXPECT_GT(now + Seconds(80), res.expires) << "Expiration not interpolated to 60s";

            loop.stop();
        } break;
        }
    });

    loop.run();

    HTTPRetryOnClockSkew.finish();
}
