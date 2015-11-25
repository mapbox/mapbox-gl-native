#include "storage.hpp"

#include <uv.h>

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

#include <future>

TEST_F(Storage, HTTPTest) {
    SCOPED_TEST(HTTPTest)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);
    util::RunLoop loop(uv_default_loop());

    const auto mainThread = uv_thread_self();

    std::unique_ptr<FileRequest> req1 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" },
               [&](Response res) {
        req1.reset();
        EXPECT_EQ(uv_thread_self(), mainThread);
        EXPECT_EQ(nullptr, res.error);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(Seconds::zero(), res.expires);
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);
        loop.stop();
        HTTPTest.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, HTTP404) {
    SCOPED_TEST(HTTP404)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);
    util::RunLoop loop(uv_default_loop());

    const auto mainThread = uv_thread_self();

    std::unique_ptr<FileRequest> req2 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/doesnotexist" },
               [&](Response res) {
        req2.reset();
        EXPECT_EQ(uv_thread_self(), mainThread);
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Cannot GET /doesnotexist\n", *res.data);
        EXPECT_EQ("HTTP status code 404", res.error->message);
        EXPECT_EQ(Seconds::zero(), res.expires);
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);
        loop.stop();
        HTTP404.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, HTTP500) {
    SCOPED_TEST(HTTP500)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);
    util::RunLoop loop(uv_default_loop());

    const auto mainThread = uv_thread_self();

    std::unique_ptr<FileRequest> req3 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/permanent-error" },
               [&](Response res) {
        req3.reset();
        EXPECT_EQ(uv_thread_self(), mainThread);
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::Server, res.error->reason);
        EXPECT_EQ(false, res.stale);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Server Error!", *res.data);
        EXPECT_EQ("HTTP status code 500", res.error->message);
        EXPECT_EQ(Seconds::zero(), res.expires);
        EXPECT_EQ(Seconds::zero(), res.modified);
        EXPECT_EQ("", res.etag);
        loop.stop();
        HTTP500.finish();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST_F(Storage, HTTPNoCallback) {
    SCOPED_TEST(HTTPNoCallback)

    using namespace mbgl;

    DefaultFileSource fs(nullptr);
    util::RunLoop loop(uv_default_loop());

    try {
        fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" },
               nullptr);
    } catch (const util::MisuseException& ex) {
        EXPECT_EQ(std::string(ex.what()), "FileSource callback can't be empty");
    } catch (const std::exception&) {
        EXPECT_TRUE(false) << "Unhandled exception.";
    }

    HTTPNoCallback.finish();
}
