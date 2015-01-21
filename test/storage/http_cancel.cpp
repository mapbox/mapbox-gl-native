#include "../util.hpp"

#include <uv.h>

#include <mbgl/storage/default/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>

#include <cmath>

TEST(Storage, HTTPCancel) {
    SCOPED_TEST(HTTPCancel)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    auto req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, uv_default_loop(), [&](const Response &) {
        ADD_FAILURE() << "Callback should not be called";
    });

    fs.cancel(req);
    HTTPCancel.finish();

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST(Storage, HTTPCancelMultiple) {
    SCOPED_TEST(HTTPCancelMultiple)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };
    auto req2 = fs.request(resource, uv_default_loop(), [&](const Response &) {
        ADD_FAILURE() << "Callback should not be called";
    });
    fs.request(resource, uv_default_loop(), [&](const Response &res) {
        EXPECT_EQ(res.status, Response::Successful);
        EXPECT_EQ(res.data, "Hello World!");
        EXPECT_EQ(res.expires, 0);
        EXPECT_EQ(res.modified, 0);
        EXPECT_EQ(res.etag, "");
        EXPECT_EQ(res.message, "");
        HTTPCancelMultiple.finish();
    });
    fs.cancel(req2);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
