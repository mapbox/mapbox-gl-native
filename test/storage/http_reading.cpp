#include "storage.hpp"

#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread_context.hpp>

#include <future>

TEST_F(Storage, HTTPTest) {
    SCOPED_TEST(HTTPTest)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    std::unique_ptr<FileRequest> req1 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" },
               [&](Response res) {
        req1.reset();
        EXPECT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Main));
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
        HTTPTest.finish();
    });

    loop.run();
}

TEST_F(Storage, HTTP404) {
    SCOPED_TEST(HTTP404)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    std::unique_ptr<FileRequest> req2 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/doesnotexist" },
               [&](Response res) {
        req2.reset();
        EXPECT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Main));
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        EXPECT_EQ("HTTP status code 404", res.error->message);
        EXPECT_FALSE(bool(res.data));
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
        HTTP404.finish();
    });

    loop.run();
}

TEST_F(Storage, HTTPTile404) {
    SCOPED_TEST(HTTPTile404)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    std::unique_ptr<FileRequest> req2 = fs.request({ Resource::Tile, "http://127.0.0.1:3000/doesnotexist" },
               [&](Response res) {
        req2.reset();
        EXPECT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Main));
        EXPECT_TRUE(res.noContent);
        EXPECT_FALSE(bool(res.error));
        EXPECT_FALSE(bool(res.data));
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
        HTTPTile404.finish();
    });

    loop.run();
}

TEST_F(Storage, HTTP200EmptyData) {
    SCOPED_TEST(HTTP200EmptyData)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    std::unique_ptr<FileRequest> req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/empty-data" },
               [&](Response res) {
        req.reset();
        EXPECT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Main));
        EXPECT_FALSE(res.noContent);
        EXPECT_FALSE(bool(res.error));
        EXPECT_EQ(*res.data, std::string());
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
        HTTP200EmptyData.finish();
    });

    loop.run();
}

TEST_F(Storage, HTTP204) {
    SCOPED_TEST(HTTP204)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    std::unique_ptr<FileRequest> req2 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/no-content" },
               [&](Response res) {
        req2.reset();
        EXPECT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Main));
        EXPECT_TRUE(res.noContent);
        EXPECT_FALSE(bool(res.error));
        EXPECT_FALSE(bool(res.data));
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
        HTTP204.finish();
    });

    loop.run();
}

TEST_F(Storage, HTTP500) {
    SCOPED_TEST(HTTP500)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    std::unique_ptr<FileRequest> req3 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/permanent-error" },
               [&](Response res) {
        req3.reset();
        EXPECT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Main));
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::Server, res.error->reason);
        EXPECT_EQ("HTTP status code 500", res.error->message);
        EXPECT_FALSE(bool(res.data));
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
        HTTP500.finish();
    });

    loop.run();
}

TEST_F(Storage, HTTPNoCallback) {
    SCOPED_TEST(HTTPNoCallback)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

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
