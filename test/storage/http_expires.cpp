#include "storage.hpp"

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
