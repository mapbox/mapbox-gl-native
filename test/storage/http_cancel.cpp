#include "storage.hpp"

#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

#include <cmath>

TEST_F(Storage, HTTPCancel) {
    SCOPED_TEST(HTTPCancel)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs(nullptr);

    auto req =
        fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" },
                   [&](Response) { ADD_FAILURE() << "Callback should not be called"; });

    req.reset();
    HTTPCancel.finish();

    loop.runOnce();
}

TEST_F(Storage, HTTPCancelMultiple) {
    SCOPED_TEST(HTTPCancelMultiple)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs(nullptr);

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };

    std::unique_ptr<FileRequest> req2 = fs.request(resource, [&](Response) {
        ADD_FAILURE() << "Callback should not be called";
    });
    std::unique_ptr<FileRequest> req = fs.request(resource, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(Seconds::zero(), res.expires);
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);
        loop.stop();
        HTTPCancelMultiple.finish();
    });
    req2.reset();

    loop.run();
}
