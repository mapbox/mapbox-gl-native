#include <mbgl/storage/network_status.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/timer.hpp>

#include <gtest/gtest.h>

using namespace mbgl;

TEST(OnlineFileSource, Cancel) {
    util::RunLoop loop;
    OnlineFileSource fs;

    fs.request({ Resource::Unknown, "http://127.0.0.1:3000/test" }, [&](Response) {
        ADD_FAILURE() << "Callback should not be called";
    });

    loop.runOnce();
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(CancelMultiple)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test" };

    std::unique_ptr<AsyncRequest> req2 = fs.request(resource, [&](Response) {
        ADD_FAILURE() << "Callback should not be called";
    });
    std::unique_ptr<AsyncRequest> req = fs.request(resource, [&](Response res) {
        req.reset();
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        loop.stop();
    });
    req2.reset();

    loop.run();
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(TemporaryError)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    const auto start = Clock::now();
    int counter = 0;

    auto req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/temporary-error" }, [&](Response res) {
        switch (counter++) {
        case 0: {
            const auto duration = std::chrono::duration<const double>(Clock::now() - start).count();
            EXPECT_GT(0.2, duration) << "Initial error request took too long";
            ASSERT_NE(nullptr, res.error);
            EXPECT_EQ(Response::Error::Reason::Server, res.error->reason);
            EXPECT_EQ("HTTP status code 500", res.error->message);
            ASSERT_FALSE(bool(res.data));
            EXPECT_FALSE(bool(res.expires));
            EXPECT_FALSE(res.mustRevalidate);
            EXPECT_FALSE(bool(res.modified));
            EXPECT_FALSE(bool(res.etag));
        } break;
        case 1: {
            const auto duration = std::chrono::duration<const double>(Clock::now() - start).count();
            EXPECT_LT(0.99, duration) << "Backoff timer didn't wait 1 second";
            EXPECT_GT(1.2, duration) << "Backoff timer fired too late";
            EXPECT_EQ(nullptr, res.error);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ("Hello World!", *res.data);
            EXPECT_FALSE(bool(res.expires));
            EXPECT_FALSE(res.mustRevalidate);
            EXPECT_FALSE(bool(res.modified));
            EXPECT_FALSE(bool(res.etag));
            loop.stop();
        } break;
        }
    });

    loop.run();
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(ConnectionError)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    const auto start = Clock::now();
    int counter = 0;
    int wait = 0;

    std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, "http://127.0.0.1:3001/" }, [&](Response res) {
        const auto duration = std::chrono::duration<const double>(Clock::now() - start).count();
        EXPECT_LT(wait - 0.01, duration) << "Backoff timer didn't wait 1 second";
        EXPECT_GT(wait + 0.3, duration) << "Backoff timer fired too late";
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::Connection, res.error->reason);
        ASSERT_FALSE(res.data.get());
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));

        if (counter == 2) {
            req.reset();
            loop.stop();
        }
        wait += (1 << counter);
        counter++;
    });

    loop.run();
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(Timeout)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    int counter = 0;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test?cachecontrol=max-age=1" };
    std::unique_ptr<AsyncRequest> req = fs.request(resource, [&](Response res) {
        counter++;
        EXPECT_EQ(nullptr, res.error);
        ASSERT_TRUE(res.data.get());
        EXPECT_EQ("Hello World!", *res.data);
        EXPECT_TRUE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));
        if (counter == 4) {
            req.reset();
            loop.stop();
        }
    });

    loop.run();

    EXPECT_EQ(4, counter);
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(RetryDelayOnExpiredTile)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    int counter = 0;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/test?expires=10000" };
    std::unique_ptr<AsyncRequest> req = fs.request(resource, [&](Response res) {
        counter++;
        EXPECT_EQ(nullptr, res.error);
        EXPECT_GT(util::now(), *res.expires);
        EXPECT_FALSE(res.mustRevalidate);
        loop.stop();
    });

    loop.run();

    EXPECT_EQ(1, counter);
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(RetryOnClockSkew)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    int counter = 0;

    const Resource resource { Resource::Unknown, "http://127.0.0.1:3000/clockskew" };
    std::unique_ptr<AsyncRequest> req1 = fs.request(resource, [&](Response res) {
        EXPECT_FALSE(res.mustRevalidate);
        switch (counter++) {
        case 0: {
            EXPECT_EQ(nullptr, res.error);
            EXPECT_GT(util::now(), *res.expires);
        } break;
        case 1: {
            EXPECT_EQ(nullptr, res.error);

            auto now = util::now();
            EXPECT_LT(now + Seconds(40), *res.expires) << "Expiration not interpolated to 60s";
            EXPECT_GT(now + Seconds(80), *res.expires) << "Expiration not interpolated to 60s";

            loop.stop();
        } break;
        }
    });

    loop.run();
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(RespectPriorExpires)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    // Very long expiration time, should never arrive.
    Resource resource1{ Resource::Unknown, "http://127.0.0.1:3000/test" };
    resource1.priorExpires = util::now() + Seconds(100000);

    std::unique_ptr<AsyncRequest> req1 = fs.request(resource1, [&](Response) {
        FAIL() << "Should never be called";
    });

    // No expiration time, should be requested immediately.
    Resource resource2{ Resource::Unknown, "http://127.0.0.1:3000/test" };

    std::unique_ptr<AsyncRequest> req2 = fs.request(resource2, [&](Response) {
        loop.stop();
    });

    // Very long expiration time, should never arrive.
    Resource resource3{ Resource::Unknown, "http://127.0.0.1:3000/test" };
    resource3.priorExpires = util::now() + Seconds(100000);

    std::unique_ptr<AsyncRequest> req3 = fs.request(resource3, [&](Response) {
        FAIL() << "Should never be called";
    });

    loop.run();
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(Load)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    const int concurrency = 50;
    const int max = 10000;
    int number = 1;

    std::unique_ptr<AsyncRequest> reqs[concurrency];

    std::function<void(int)> req = [&](int i) {
        const auto current = number++;
        reqs[i] = fs.request({ Resource::Unknown,
                     std::string("http://127.0.0.1:3000/load/") + util::toString(current) },
                   [&, i, current](Response res) {
            reqs[i].reset();
            EXPECT_EQ(nullptr, res.error);
            ASSERT_TRUE(res.data.get());
            EXPECT_EQ(std::string("Request ") +  util::toString(current), *res.data);
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

// Test for https://github.com/mapbox/mapbox-gl-native/issues/2123
//
// A request is made. While the request is in progress, the network status changes. This should
// trigger an immediate retry of all requests that are not in progress. This test makes sure that
// we don't accidentally double-trigger the request.

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(NetworkStatusChange)) {
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
         EXPECT_FALSE(res.mustRevalidate);
         EXPECT_FALSE(bool(res.modified));
         EXPECT_FALSE(bool(res.etag));
         loop.stop();
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
TEST(OnlineFileSource, TEST_REQUIRES_SERVER(NetworkStatusChangePreempt)) {
    util::RunLoop loop;
    OnlineFileSource fs;
    fs.pause();

    const auto start = Clock::now();
    int counter = 0;

    const Resource resource{ Resource::Unknown, "http://127.0.0.1:3001/test" };
    std::unique_ptr<AsyncRequest> req = fs.request(resource, [&](Response res) {
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
        ASSERT_FALSE(res.data.get());
        EXPECT_FALSE(bool(res.expires));
        EXPECT_FALSE(res.mustRevalidate);
        EXPECT_FALSE(bool(res.modified));
        EXPECT_FALSE(bool(res.etag));

        if (counter++ == 1) {
            req.reset();
            loop.stop();
        }
    });

    // After 400 milliseconds, we're going to trigger a NetworkStatus change.
    util::Timer reachableTimer;
    reachableTimer.start(Milliseconds(400), Duration::zero(), [] () {
        mbgl::NetworkStatus::Reachable();
    });

    fs.resume();
    loop.run();
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(NetworkStatusOnlineOffline)) {
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
    });

    loop.run();
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(RateLimitStandard)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    auto req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/rate-limit?std=true" }, [&](Response res) {
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::RateLimit, res.error->reason);
        ASSERT_EQ(true, bool(res.error->retryAfter));
        ASSERT_LT(util::now(), res.error->retryAfter);
        loop.stop();
    });

    loop.run();
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(RateLimitMBX)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    auto req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/rate-limit?mbx=true" }, [&](Response res) {
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::RateLimit, res.error->reason);
        ASSERT_EQ(true, bool(res.error->retryAfter));
        ASSERT_LT(util::now(), res.error->retryAfter);
        loop.stop();
    });

    loop.run();
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(RateLimitDefault)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    auto req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/rate-limit" }, [&](Response res) {
        ASSERT_NE(nullptr, res.error);
        EXPECT_EQ(Response::Error::Reason::RateLimit, res.error->reason);
        ASSERT_FALSE(res.error->retryAfter);
        loop.stop();
    });

    loop.run();
}

TEST(OnlineFileSource, GetBaseURLAndAccessTokenWhilePaused) {
    util::RunLoop loop;
    OnlineFileSource fs;

    fs.pause();

    auto baseURL = "http://url";
    auto accessToken = "access_token";

    fs.setProperty(API_BASE_URL_KEY, baseURL);
    fs.setProperty(ACCESS_TOKEN_KEY, accessToken);

    EXPECT_EQ(*fs.getProperty(API_BASE_URL_KEY).getString(), baseURL);
    EXPECT_EQ(*fs.getProperty(ACCESS_TOKEN_KEY).getString(), accessToken);
}

TEST(OnlineFileSource, ChangeAPIBaseURL){
    util::RunLoop loop;
    OnlineFileSource fs;

    EXPECT_EQ(mbgl::util::API_BASE_URL, *fs.getProperty(API_BASE_URL_KEY).getString());
    const std::string customURL = "test.domain";
    fs.setProperty(API_BASE_URL_KEY, customURL);
    EXPECT_EQ(customURL, *fs.getProperty(API_BASE_URL_KEY).getString());
}


TEST(OnlineFileSource, TEST_REQUIRES_SERVER(LowHighPriorityRequests)) {
    util::RunLoop loop;
    OnlineFileSource fs;
    std::size_t response_counter = 0;
    const std::size_t NUM_REQUESTS = 3;

    NetworkStatus::Set(NetworkStatus::Status::Offline);
    fs.setProperty(MAX_CONCURRENT_REQUESTS_KEY, 1u);
    // After DefaultFileSource was split, OnlineFileSource lives on a separate
    // thread. Pause OnlineFileSource, so that messages are queued for processing.
    fs.pause();

    // First regular request.
    Resource regular1{Resource::Unknown, "http://127.0.0.1:3000/load/1"};
    std::unique_ptr<AsyncRequest> req_0 = fs.request(regular1, [&](Response) {
        response_counter++;
        req_0.reset();
    });

    // Low priority request that will be queued and should be requested last.
    Resource low_prio{Resource::Unknown, "http://127.0.0.1:3000/load/2"};
    low_prio.setPriority(Resource::Priority::Low);
    std::unique_ptr<AsyncRequest> req_1 = fs.request(low_prio, [&](Response) {
        response_counter++;
        req_1.reset();
        EXPECT_EQ(response_counter, NUM_REQUESTS); // make sure this is responded last
        loop.stop();
    });

    // Second regular priority request that should de-preoritize low priority request.
    Resource regular2{ Resource::Unknown, "http://127.0.0.1:3000/load/3" };
    std::unique_ptr<AsyncRequest> req_2 = fs.request(regular2, [&](Response) {
        response_counter++;
        req_2.reset();
    });

    fs.resume();
    NetworkStatus::Set(NetworkStatus::Status::Online);
    loop.run();
}


TEST(OnlineFileSource, TEST_REQUIRES_SERVER(LowHighPriorityRequestsMany)) {
    util::RunLoop loop;
    OnlineFileSource fs;
    int response_counter = 0;
    int correct_low = 0;
    int correct_regular = 0;

    NetworkStatus::Set(NetworkStatus::Status::Offline);
    fs.setProperty(MAX_CONCURRENT_REQUESTS_KEY, 1u);
    fs.pause();

    std::vector<std::unique_ptr<AsyncRequest>> collector;

    for (int num_reqs = 0; num_reqs < 20; num_reqs++) {
        if (num_reqs % 2 == 0) {
            std::unique_ptr<AsyncRequest> req = fs.request({ Resource::Unknown, "http://127.0.0.1:3000/load/" + std::to_string(num_reqs) }, [&](Response) {
                response_counter++;

                if (response_counter <= 10) { // count the high priority requests that arrive late correctly
                    correct_regular++;
                }
            });
            collector.push_back(std::move(req));
        }
        else {
            Resource resource = { Resource::Unknown, "http://127.0.0.1:3000/load/" + std::to_string(num_reqs) };
            resource.setPriority(Resource::Priority::Low);

            std::unique_ptr<AsyncRequest> req = fs.request(std::move(resource), [&](Response) {
                response_counter++;

                if (response_counter > 10) { // count the low priority requests that arrive late correctly
                    correct_low++;
                }

                // stop and check correctness after last low priority request is responded
                if (20 == response_counter) {
                    loop.stop();
                    for (auto& collected_req : collector) {
                        collected_req.reset();
                    }
                    ASSERT_TRUE(correct_low >= 9);
                    ASSERT_TRUE(correct_regular >= 9);
                }
            });
            collector.push_back(std::move(req));
        }
    }

    fs.resume();
    NetworkStatus::Set(NetworkStatus::Status::Online);
    loop.run();
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(MaximumConcurrentRequests)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    ASSERT_EQ(*fs.getProperty(MAX_CONCURRENT_REQUESTS_KEY).getUint(), 20u);

    fs.setProperty(MAX_CONCURRENT_REQUESTS_KEY, 10u);
    ASSERT_EQ(*fs.getProperty(MAX_CONCURRENT_REQUESTS_KEY).getUint(), 10u);
}

TEST(OnlineFileSource, TEST_REQUIRES_SERVER(RequestSameUrlMultipleTimes)) {
    util::RunLoop loop;
    OnlineFileSource fs;

    int count = 0;
    std::vector<std::unique_ptr<AsyncRequest>> requests;

    for (int i = 0; i < 100; ++i) {
        requests.emplace_back(fs.request({ Resource::Unknown, "http://127.0.0.1:3000/load" }, [&](Response) {
            if (++count == 100) {
                loop.stop();
            }
        }));
    }

    loop.run();
}
