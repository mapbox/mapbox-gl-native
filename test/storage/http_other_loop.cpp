#include "storage.hpp"

#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, TEST_REQUIRES_SERVER(HTTPOtherLoop)) {
    SCOPED_TEST(HTTPOtherLoop)

    using namespace mbgl;

    // This file source launches a separate thread to do the processing.
    util::RunLoop loop;
    OnlineFileSource fs;

    std::unique_ptr<FileRequest> req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" },
               [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
        HTTPOtherLoop.finish();
    });

    loop.run();
}
