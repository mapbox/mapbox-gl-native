#include "storage.hpp"

#include <mbgl/map/environment.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <uv.h>

#include <thread>

TEST_F(Storage, HTTPReading) {
    SCOPED_TEST(HTTPTest)
    SCOPED_TEST(HTTP404)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    Environment env(fs);
    EnvironmentScope scope(env, ThreadType::Test, TEST_CASE_NAME(), uv_default_loop());

    const auto mainThread = uv_thread_self();

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, std::this_thread::get_id(),
               [&](const Response &res) {
        EXPECT_EQ(uv_thread_self(), mainThread);
        EXPECT_EQ(Response::Successful, res.status);
        EXPECT_EQ("Hello World!", res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        EXPECT_EQ("", res.message);
        HTTPTest.finish();
    });

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/doesnotexist" },
            std::this_thread::get_id(), [&](const Response &res) {
        EXPECT_EQ(uv_thread_self(), mainThread);
        EXPECT_EQ(Response::Error, res.status);
        EXPECT_EQ("HTTP status code 404", res.message);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        HTTP404.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, HTTPNoCallback) {
    SCOPED_TEST(HTTPTest)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    Environment env(fs);
    EnvironmentScope scope(env, ThreadType::Test, TEST_CASE_NAME(), uv_default_loop());

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, std::this_thread::get_id(),
               nullptr);

    HTTPTest.finish();

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, HTTPNoCallbackNoLoop) {
    SCOPED_TEST(HTTPTest)

    using namespace mbgl;

    DefaultFileSource fs(nullptr, uv_default_loop());

    Environment env(fs);
    EnvironmentScope scope(env, ThreadType::Test, TEST_CASE_NAME(), uv_default_loop());

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, nullptr);

    HTTPTest.finish();

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
