#include <mbgl/storage/local_file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/run_loop.hpp>

#include <climits>
#include <gtest/gtest.h>

#if defined(WIN32)
#include <Windows.h>
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif /* PATH_MAX */
#else
#include <unistd.h>
#endif

namespace {

std::string toAbsoluteURL(const std::string& fileName) {
    char buff[PATH_MAX + 1];
    char* cwd = getcwd( buff, PATH_MAX + 1 );
    std::string url = { "file://" + std::string(cwd) + "/test/fixtures/storage/assets/" + fileName };
    assert(url.size() <= PATH_MAX);
    return url;
}

} // namespace

using namespace mbgl;

TEST(LocalFileSource, AcceptsURL) {
    LocalFileSource fs;
    EXPECT_TRUE(fs.canRequest(Resource::style("file://empty")));
    EXPECT_TRUE(fs.canRequest(Resource::style("file:///test")));
    EXPECT_FALSE(fs.canRequest(Resource::style("flie://foo")));
    EXPECT_FALSE(fs.canRequest(Resource::style("file:")));
    EXPECT_FALSE(fs.canRequest(Resource::style("style.json")));
    EXPECT_FALSE(fs.canRequest(Resource::style("")));
}

TEST(LocalFileSource, EmptyFile) {
    util::RunLoop loop;

    LocalFileSource fs;

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, toAbsoluteURL("empty") }, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("", *res.data);
        loop.stop();
    });

    loop.run();
}

TEST(LocalFileSource, NonEmptyFile) {
    util::RunLoop loop;

    LocalFileSource fs;

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, toAbsoluteURL("nonempty") }, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("content is here\n", *res.data);
        loop.stop();
    });

    loop.run();
}

TEST(LocalFileSource, NonExistentFile) {
    util::RunLoop loop;

    LocalFileSource fs;

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, toAbsoluteURL("does_not_exist") }, [&](Response res) {
        req.reset();
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        ASSERT_FALSE(res.data.get());
        // Do not assert on platform-specific error message.
        loop.stop();
    });

    loop.run();
}

TEST(LocalFileSource, InvalidURL) {
    util::RunLoop loop;

    LocalFileSource fs;

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, "test://wrong-scheme" }, [&](Response res) {
        req.reset();
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::Other, res.error->reason);
        EXPECT_EQ("Invalid file URL", res.error->message);
        ASSERT_FALSE(res.data.get());
        loop.stop();
    });

    loop.run();
}

TEST(LocalFileSource, ReadDirectory) {
    util::RunLoop loop;

    LocalFileSource fs;

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, toAbsoluteURL("directory") }, [&](Response res) {
        req.reset();
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        ASSERT_FALSE(res.data.get());
        // Do not assert on platform-specific error message.
        loop.stop();
    });

    loop.run();
}

TEST(LocalFileSource, URLEncoding) {
    util::RunLoop loop;

    LocalFileSource fs;

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, toAbsoluteURL("%6eonempty") }, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("content is here\n", *res.data);
        loop.stop();
    });

    loop.run();
}

TEST(LocalFileSource, URLLimit) {
    util::RunLoop loop;

    size_t length = PATH_MAX - toAbsoluteURL("").size();
    LocalFileSource fs;
    char* filename = new char[length];
    memset(filename, 'x', length);

    std::string url(filename, length);

    delete[] filename;

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, toAbsoluteURL(url) }, [&](Response res) {
        req.reset();
        ASSERT_NE(nullptr, res.error);
#if defined(_MSC_VER) && !defined(__clang__)
        // Microsoft Visual Studio defines PATH_MAX as 260, less than the limit to trigger an error with reason "Other"
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
#else
        EXPECT_EQ(Response::Error::Reason::Other, res.error->reason);
#endif
        ASSERT_FALSE(res.data.get());
        loop.stop();
    });

    loop.run();
}
