#include "storage.hpp"

#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

// Test for https://github.com/mapbox/mapbox-gl-native/issues/2123
//
// A request is made. While the request is in progress, the network status changes. This should
// trigger an immediate retry of all requests that are not in progress. This test makes sure that
// we don't accidentally double-trigger the request.

TEST_F(Storage, TEST_REQUIRES_SERVER(HTTPNetworkStatusChange)) {
    SCOPED_TEST(HTTPNetworkStatusChange)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/delayed" };

    // This request takes 200 milliseconds to answer.
    std::unique_ptr<AsyncRequest> req = fs.request(resource, [&](Response res) {
         req.reset();
         EXPECT_EQ(nullptr, res.error);
         ASSERT_TRUE(res.data.get());
         EXPECT_EQ("Response", *res.data);
         EXPECT_FALSE(bool(res.expires));
         EXPECT_FALSE(bool(res.modified));
         EXPECT_FALSE(bool(res.etag));
         loop.stop();
         HTTPNetworkStatusChange.finish();
    });

    // After 50 milliseconds, we're going to trigger a NetworkStatus change.
    util::Timer reachableTimer;
    reachableTimer.start(Milliseconds(50), Duration::zero(), [] () {
        mbgl::NetworkStatus::Reachable();
    });

    loop.run();
}

// Tests that a change in network status preempts requests that failed due to connection or
// reachability issues.
TEST_F(Storage, TEST_REQUIRES_SERVER(HTTPNetworkStatusChangePreempt)) {
    SCOPED_TEST(HTTPNetworkStatusChangePreempt)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    const auto start = Clock::now();

    const Resource resource{ Resource::Unknown, "http://127.0.0.1:3001/test" };
    std::unique_ptr<AsyncRequest> req = fs.request(resource, [&](Response res) {
        static int counter = 0;
        const auto duration = std::chrono::duration<const double>(Clock::now() - start).count();
        if (counter == 0) {
            EXPECT_GT(0.2, duration) << "Response came in too late";
        } else if (counter == 1) {
            EXPECT_LT(0.39, duration) << "Preempted retry triggered too early";
            EXPECT_GT(0.6, duration) << "Preempted retry triggered too late";
        } else if (counter > 1) {
            FAIL() << "Retried too often";
        }
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

        if (counter++ == 1) {
            req.reset();
            loop.stop();
            HTTPNetworkStatusChangePreempt.finish();
        }
    });

    // After 400 milliseconds, we're going to trigger a NetworkStatus change.
    util::Timer reachableTimer;
    reachableTimer.start(Milliseconds(400), Duration::zero(), [] () {
        mbgl::NetworkStatus::Reachable();
    });

    loop.run();
}

TEST_F(Storage, TEST_REQUIRES_SERVER(HTTPNetworkStatusOnlineOffline)) {
    SCOPED_TEST(HTTPNetworkStatusOnlineOffline)

    using namespace mbgl;

    util::RunLoop loop;
    OnlineFileSource fs;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };

    EXPECT_EQ(NetworkStatus::Get(), NetworkStatus::Status::Online) << "Default status should be Online";
    NetworkStatus::Set(NetworkStatus::Status::Offline);

    util::Timer onlineTimer;
    onlineTimer.start(Milliseconds(100), Duration::zero(), [&] () {
        NetworkStatus::Set(NetworkStatus::Status::Online);
    });

    std::unique_ptr<AsyncRequest> req = fs.request(resource, [&](Response res) {
        req.reset();

        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());

        EXPECT_EQ(NetworkStatus::Get(), NetworkStatus::Status::Online) << "Triggered before set back to Online";

        loop.stop();
        HTTPNetworkStatusOnlineOffline.finish();
    });

    loop.run();
}
