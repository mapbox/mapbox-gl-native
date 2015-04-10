#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/thread.hpp>

TEST_F(Storage, HTTPOtherLoop) {
    SCOPED_TEST(HTTPOtherLoop)

    using namespace mbgl;

    // This file source launches a separate thread to do the processing.
    util::Thread<DefaultFileSource> fs(nullptr);

    auto &env = *static_cast<const Environment *>(nullptr);

    fs->request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, uv_default_loop(), env,
               [&](const Response &res) {
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ("Hello World!", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);
        HTTPOtherLoop.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
