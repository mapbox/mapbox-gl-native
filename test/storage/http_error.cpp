#include "storage.hpp"

#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

#include <cmath>

TEST_F(Storage, TEST_REQUIRES_SERVER(HTTPTemporaryError)) {
    SCOPED_TEST(HTTPTemporaryError)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    const auto start = Clock::now();

    std::unique_ptr<AsyncRequest> req1 = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/temporary-error" }, [&](Response res) {
        static int counter = 0;
        switch (counter++) {
        case 0: {
            const auto duration = std::chrono::duration<const double>(Clock::now() - start).count();
            EXPECT_GT(0.2, duration) << "Initial error request took too long";
            ASSERT_NE(nullptr, res.error);
            EXPECT_EQ(Response::Error::Reason::Server, res.error->reason);
            EXPECT_EQ("HTTP status code 500", res.error->message);
            ASSERT_FALSE(bool(res.data));
            EXPECT_FALSE(bool(res.expires));
            EXPECT_FALSE(bool(res.modified));
            EXPECT_FALSE(bool(res.etag));
        } break;
        case 1: {
            req1.reset();
            const auto duration = std::chrono::duration<const double>(Clock::now() - start).count();
            EXPECT_LT(0.99, duration) << "Backoff timer didn't wait 1 second";
            EXPECT_GT(1.2, duration) << "Backoff timer fired too late";
            EXPECT_EQ(nullptr, res.error);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("Hello World!", *res.data);
            EXPECT_FALSE(bool(res.expires));
            EXPECT_FALSE(bool(res.modified));
            EXPECT_FALSE(bool(res.etag));
            loop.stop();
            HTTPTemporaryError.finish();
        } break;
        }
    });

    loop.run();
}

TEST_F(Storage, TEST_REQUIRES_SERVER(HTTPConnectionError)) {
    SCOPED_TEST(HTTPConnectionError)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    const auto start = Clock::now();

    std::unique_ptr<AsyncRequest> req2 = fs.request({ Resource::Unknown, "http://127.0.0.1:3001/" }, [&](Response res) {
        static int counter = 0;
        static int wait = 0;
        const auto duration = std::chrono::duration<const double>(Clock::now() - start).count();
        EXPECT_LT(wait - 0.01, duration) << "Backoff timer didn't wait 1 second";
        EXPECT_GT(wait + 0.2, duration) << "Backoff timer fired too late";
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::Connection, res.error->reason);
#ifdef MBGL_HTTP_NSURL
        EXPECT_TRUE(res.error->message ==
                     "The operation couldn’t be completed. (NSURLErrorDomain error -1004.)" ||
                 res.error->message == "Could not connect to the server.")
         << "Full message is: \"" << res.error->message << "\"";
#elif MBGL_HTTP_CURL
        const std::string prefix { "Couldn't connect to server: " };
        EXPECT_STREQ(prefix.c_str(), res.error->message.substr(0, prefix.size()).c_str()) << "Full message is: \"" << res.error->message << "\"";
#else
        FAIL();
#endif
        ASSERT_FALSE(res.data.get());
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));

        if (counter == 2) {
            req2.reset();
            loop.stop();
            HTTPConnectionError.finish();
        }
        wait += (1 << counter);
        counter++;
    });

    loop.run();
}
