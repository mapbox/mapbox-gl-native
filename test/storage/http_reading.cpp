#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/thread.hpp>

TEST_F(Storage, HTTPReading) {
    SCOPED_TEST(HTTPTest)
    SCOPED_TEST(HTTP404)

    using namespace mbgl;

    util::Thread<DefaultFileSource> fs(nullptr);

    auto &env = *static_cast<const Environment *>(nullptr);

    const auto mainThread = uv_thread_self();

    fs->request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, uv_default_loop(), env,
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

    fs->request({ Resource::Unknown, "http://127.0.0.1:3000/doesnotexist" }, uv_default_loop(),
               env, [&](const Response &res) {
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

    util::Thread<DefaultFileSource> fs(nullptr);

    auto &env = *static_cast<const Environment *>(nullptr);

    fs->request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, uv_default_loop(), env,
               nullptr);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    HTTPTest.finish();
}

TEST_F(Storage, HTTPNoCallbackNoLoop) {
    SCOPED_TEST(HTTPTest)

    using namespace mbgl;

    util::Thread<DefaultFileSource> fs(nullptr);

    auto &env = *static_cast<const Environment *>(nullptr);

    fs->request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, env, nullptr);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    HTTPTest.finish();
}
