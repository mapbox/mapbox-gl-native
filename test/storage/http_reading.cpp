#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/exception.hpp>

#include <future>

TEST_F(Storage, HTTPReading) {
    SCOPED_TEST(HTTPTest)
    SCOPED_TEST(HTTP404)
    SCOPED_TEST(HTTP500)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);

    const auto mainThread = uv_thread_self();

    Request* req1 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, uv_default_loop(),
               [&](const Response &res) {
        fs.cancel(req1);
        EXPECT_EQ(uv_thread_self(), mainThread);
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        HTTPTest.finish();
    });

    Request* req2 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/doesnotexist" }, uv_default_loop(),
               [&](const Response &res) {
        fs.cancel(req2);
        EXPECT_EQ(uv_thread_self(), mainThread);
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Cannot GET /doesnotexist\n", *res.data);
        EXPECT_EQ("HTTP status code 404", res.error->message);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        HTTP404.finish();
    });

    Request* req3 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/permanent-error" }, uv_default_loop(),
               [&](const Response &res) {
        fs.cancel(req3);
        EXPECT_EQ(uv_thread_self(), mainThread);
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::Server, res.error->reason);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Server Error!", *res.data);
        EXPECT_EQ("HTTP status code 500", res.error->message);
        EXPECT_EQ(0, res.expires);
        EXPECT_EQ(0, res.modified);
        EXPECT_EQ("", res.etag);
        HTTP500.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, HTTPNoCallback) {
    SCOPED_TEST(HTTPTest)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);

    try {
        fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, uv_default_loop(),
               nullptr);
    } catch (const util::MisuseException& ex) {
        EXPECT_EQ(std::string(ex.what()), "FileSource callback can't be empty");
    } catch (const std::exception&) {
        EXPECT_TRUE(false) << "Unhandled exception.";
    }

    HTTPTest.finish();
}
