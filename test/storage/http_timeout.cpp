#include "storage.hpp"

#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, HTTPTimeout) {
    SCOPED_TEST(HTTPTimeout)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs(nullptr);

    int counter = 0;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test?cachecontrol=max-age=1" };
    std::unique_ptr<FileRequest> req = fs.request(resource, [&](Response res) {
        counter++;
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_LT(Seconds::zero(), res.expires);
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);
        if (counter == 4) {
            req.reset();
            loop.stop();
            HTTPTimeout.finish();
        }
    });

    loop.run();

    EXPECT_EQ(4, counter);
}
