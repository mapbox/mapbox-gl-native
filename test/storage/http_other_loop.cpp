#include "storage.hpp"

#include <mbgl/map/environment.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <uv.h>

#include <thread>

TEST_F(Storage, HTTPOtherLoop) {
    SCOPED_TEST(HTTPOtherLoop)

    using namespace mbgl;

    // This file source launches a separate thread to do the processing.
    DefaultFileSource fs(nullptr);

    Environment env(fs);
    EnvironmentScope scope(env, ThreadType::Test, TEST_CASE_NAME(), uv_default_loop());

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, std::this_thread::get_id(),
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
