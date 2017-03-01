#include <mbgl/test/util.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/util/http_timeout.hpp>

using namespace mbgl;
using namespace mbgl::http;

TEST(HttpRetry, OtherError) {
    // Non-retryable
    ASSERT_EQ(Duration::max(), errorRetryTimeout(Response::Error::Reason::Other, 1));
}

TEST(HttpRetry, NotFound) {
    // Non-retryable
    ASSERT_EQ(Duration::max(), errorRetryTimeout(Response::Error::Reason::NotFound, 1));
}

TEST(HttpRetry, ServerError) {
    // 1-3 failures -> 1 sec
    ASSERT_EQ(Seconds(1), errorRetryTimeout(Response::Error::Reason::Server, 1));
    ASSERT_EQ(Seconds(1), errorRetryTimeout(Response::Error::Reason::Server, 3));

    // After 3, exponential backoff
    ASSERT_EQ(Seconds(2), errorRetryTimeout(Response::Error::Reason::Server, 4));
    ASSERT_EQ(Seconds(1u << 31), errorRetryTimeout(Response::Error::Reason::Server, 50));
}

TEST(HttpRetry, ConnectionError) {
    // Exponential backoff
    ASSERT_EQ(Seconds(1), errorRetryTimeout(Response::Error::Reason::Server, 1));
    ASSERT_EQ(Seconds(1u << 31), errorRetryTimeout(Response::Error::Reason::Connection, 50));
}

TEST(HttpRetry, RateLimit) {
    // Pre-set value from header
    ASSERT_EQ(Seconds(1), errorRetryTimeout(Response::Error::Reason::Server, 1, { util::now() + Seconds(1) }));

    // Default
    ASSERT_EQ(Seconds(5), errorRetryTimeout(Response::Error::Reason::RateLimit, 1, {}));
}

TEST(HttpRetry, ExpiredInitial) {
    // 1 sec timeout
    ASSERT_EQ(Seconds(1), expirationTimeout({ util::now() + Seconds(1) }, 0));
}

TEST(HttpRetry, ExpiredSubsequent) {
    // exponential backoff
    ASSERT_EQ(Seconds(1), expirationTimeout({}, 1));
    ASSERT_EQ(Seconds(1u << 31), expirationTimeout({}, 50));
}

TEST(HttpRetry, ExpiredNotSet) {
    // No expires header set
    ASSERT_EQ(Duration::max(), expirationTimeout({}, 0));
}
