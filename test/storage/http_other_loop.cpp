#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

TEST_F(Storage, HTTPOtherLoop) {
    SCOPED_TEST(HTTPOtherLoop)

    using namespace mbgl;

    // This file source launches a separate thread to do the processing.
    DefaultFileSource fs(nullptr);
    util::RunLoop loop(uv_default_loop());

    std::unique_ptr<FileRequest> req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" },
               [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(Seconds::zero(), res.expires);
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);
        loop.stop();
        HTTPOtherLoop.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
