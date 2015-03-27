#include "storage.hpp"

#include <mbgl/map/environment.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>

#include <uv.h>

#include <cmath>
#include <thread>

TEST_F(Storage, HTTPCancelEnvironment) {
    SCOPED_TEST(HTTPCanceledEnvironment)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/delayed" };

    // Environment 1
    Environment env1(fs);
    EnvironmentScope scope1(env1, ThreadType::Test, TEST_CASE_NAME(), uv_default_loop());

    fs.request(resource, std::this_thread::get_id(), [&](const Response &res) {
        // This environment gets aborted below. This means the request is marked as failing and
        // will return an error here.
        EXPECT_EQ(Response::Error, res.status);
        EXPECT_EQ("", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("Environment is terminating", res.message);
        HTTPCanceledEnvironment.finish();
    });

    auto thread = std::thread([&]() {
        SCOPED_TEST(HTTPRetainedEnvironment)
        uv_loop_t* loop = uv_loop_new();

        // Environment 2
        Environment env2(fs);
        EnvironmentScope scope2(env2, ThreadType::Test, TEST_CASE_NAME(), loop);

        fs.request(resource, std::this_thread::get_id(), [&](const Response &res) {
            // The same request as above, but in a different environment which doesn't get aborted. This
            // means the request should succeed.
            EXPECT_EQ(Response::Successful, res.status);
            EXPECT_EQ("Response", res.data);
            EXPECT_EQ(0, res.expires);
            EXPECT_EQ(0, res.modified);
            EXPECT_EQ("", res.etag);
            EXPECT_EQ("", res.message);
            HTTPRetainedEnvironment.finish();
        });

        uv_run(loop, UV_RUN_DEFAULT);
        uv_loop_delete(loop);
    });

    fs.abort(env1.getID());

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    thread.join();
}
