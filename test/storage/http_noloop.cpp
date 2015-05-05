#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/uv.hpp>

TEST_F(Storage, HTTPNoLoop) {
    SCOPED_TEST(HTTPNoLoop)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);

    const auto mainThread = uv_thread_self();

    // Async handle that keeps the main loop alive until the thread finished
    auto async = new uv_async_t;
    uv_async_init(uv_default_loop(), async, [] (uv_async_t *as, int) {
        uv::close(as);
    });

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/temporary-error" }, nullptr,
               [&](const Response &res) {
        EXPECT_NE(uv_thread_self(), mainThread) << "Response was called in the same thread";
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ("Hello World!", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);
        HTTPNoLoop.finish();

        uv_async_send(async);
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
