#include "storage.hpp"

#include <mbgl/map/environment.hpp>
#include <mbgl/storage/default/request.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>

#include <uv.h>

#include <cmath>
#include <thread>

TEST_F(Storage, HTTPCancel) {
    SCOPED_TEST(HTTPCancel)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    Environment env(fs);
    EnvironmentScope scope(env, ThreadType::Test, TEST_CASE_NAME(), uv_default_loop());

    Request* req =
        fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, std::this_thread::get_id(),
                   [&](const Response &) { ADD_FAILURE() << "Callback should not be called"; });

    fs.cancel(req);
    HTTPCancel.finish();

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, HTTPCancelMultiple) {
    SCOPED_TEST(HTTPCancelMultiple)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    Environment env(fs);
    EnvironmentScope scope(env, ThreadType::Test, TEST_CASE_NAME(), uv_default_loop());

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };

    auto req2 = fs.request(resource, std::this_thread::get_id(), [&](const Response &) {
        ADD_FAILURE() << "Callback should not be called";
    });
    fs.request(resource, std::this_thread::get_id(), [&](const Response &res) {
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ("Hello World!", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);
        HTTPCancelMultiple.finish();
    });
    fs.cancel(req2);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
