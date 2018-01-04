#include <mbgl/test/util.hpp>
#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;

TEST(HTTPFileSource, TEST_REQUIRES_SERVER(Cancel)) {
    util::RunLoop loop;
    HTTPFileSource fs;

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, [&](Response) {
        ADD_FAILURE() << "Callback should not be called";
    });

    loop.runOnce();
}

TEST(HTTPFileSource, TEST_REQUIRES_SERVER(HTTP200)) {
    util::RunLoop loop;
    HTTPFileSource fs;

    auto req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, [&](Response res) {
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

TEST(HTTPFileSource, TEST_REQUIRES_SERVER(HTTP404)) {
    util::RunLoop loop;
    HTTPFileSource fs;

    auto req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/doesnotexist" }, [&](Response res) {
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::NotFound, res.error->reason);
        EXPECT_EQ("HTTP status code 404", res.error->message);
        EXPECT_FALSE(bool(res.data));
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

TEST(HTTPFileSource, TEST_REQUIRES_SERVER(HTTPTile404)) {
    util::RunLoop loop;
    HTTPFileSource fs;

    auto req = fs.request({ Resource::Tile, "http://127.0.0.1:3000/doesnotexist" }, [&](Response res) {
        EXPECT_TRUE(res.noContent);
        EXPECT_FALSE(bool(res.error));
        EXPECT_FALSE(bool(res.data));
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

TEST(HTTPFileSource, TEST_REQUIRES_SERVER(HTTP200EmptyData)) {
    util::RunLoop loop;
    HTTPFileSource fs;

    auto req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/empty-data" }, [&](Response res) {
        EXPECT_FALSE(res.noContent);
        EXPECT_FALSE(bool(res.error));
        EXPECT_EQ(*res.data, std::string());
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

TEST(HTTPFileSource, TEST_REQUIRES_SERVER(HTTP204)) {
    util::RunLoop loop;
    HTTPFileSource fs;

    auto req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/no-content" }, [&](Response res) {
        EXPECT_TRUE(res.noContent);
        EXPECT_FALSE(bool(res.error));
        EXPECT_FALSE(bool(res.data));
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

TEST(HTTPFileSource, TEST_REQUIRES_SERVER(HTTP500)) {
    util::RunLoop loop;
    HTTPFileSource fs;

    auto req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/permanent-error" }, [&](Response res) {
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::Server, res.error->reason);
        EXPECT_EQ("HTTP status code 500", res.error->message);
        EXPECT_FALSE(bool(res.data));
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

TEST(HTTPFileSource, TEST_REQUIRES_SERVER(ExpiresParsing)) {
    util::RunLoop loop;
    HTTPFileSource fs;

    auto req = fs.request({ Resource::Unknown,
                 "http://127.0.0.1:3000/test?modified=1420794326&expires=1420797926&etag=foo" }, [&](Response res) {
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_EQ(Timestamp{ Seconds(1420797926) }, res.expires);
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_EQ(Timestamp{ Seconds(1420794326) }, res.modified);
        EXPECT_EQ("foo", *res.etag);
        loop.stop();
    });

    loop.run();
}

TEST(HTTPFileSource, TEST_REQUIRES_SERVER(CacheControlParsing)) {
    util::RunLoop loop;
    HTTPFileSource fs;

    auto req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test?cachecontrol=max-age=120" }, [&](Response res) {
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_GT(Seconds(2), util::abs(*res.expires - util::now() - Seconds(120))) << "Expiration date isn't about 120 seconds in the future";
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });

    loop.run();
}

TEST(HTTPFileSource, TEST_REQUIRES_SERVER(Load)) {
    util::RunLoop loop;
    HTTPFileSource fs;

    const int concurrency = 50;
    const int max = 10000;
    int number = 1;

    std::unique_ptr<AsyncRequest> reqs[concurrency];

    std::function<void(int)> req = [&](int i) {
        const auto current = number++;
        reqs[i] = fs.request({ Resource::Unknown,
                     std::string("http://127.0.0.1:3000/load/") + std::to_string(current) },
                   [&, i, current](Response res) {
            reqs[i].reset();
            EXPECT_EQ(nullptr, res.error);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ(std::string("Request ") +  std::to_string(current), *res.data);
            EXPECT_FALSE(bool(res.expires));
            EXPECT_FALSE(res.mustRevalidate);
            EXPECT_FALSE(bool(res.modified));
            EXPECT_FALSE(bool(res.etag));

            if (number <= max) {
                req(i);
            } else if (current == max) {
                loop.stop();
            }
        });
    };

    for (int i = 0; i < concurrency; i++) {
        req(i);
    }

    loop.run();
}
