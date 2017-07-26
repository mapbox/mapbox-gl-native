#include <mbgl/test/util.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/util/http_timeout.hpp>

using namespace mbgl;
using namespace mbgl::http;

TEST(HttpRetry, OtherError) {
    // Non-retryable
    ASSERT_EQ(Duration::max(), errorRetryTimeout(ResourceStatus::OtherError, 1));
}

TEST(HttpRetry, NotFound) {
    // Non-retryable
    ASSERT_EQ(Duration::max(), errorRetryTimeout(ResourceStatus::NotFoundError, 1));
}

TEST(HttpRetry, ServerError) {
    // 1-3 failures -> 1 sec
    ASSERT_EQ(Seconds(1), errorRetryTimeout(ResourceStatus::ServerError, 1));
    ASSERT_EQ(Seconds(1), errorRetryTimeout(ResourceStatus::ServerError, 3));

    // After 3, exponential backoff
    ASSERT_EQ(Seconds(2), errorRetryTimeout(ResourceStatus::ServerError, 4));
    ASSERT_EQ(Seconds(1u << 31), errorRetryTimeout(ResourceStatus::ServerError, 50));
}

TEST(HttpRetry, ConnectionError) {
    // Exponential backoff
    ASSERT_EQ(Seconds(1), errorRetryTimeout(ResourceStatus::ServerError, 1));
    ASSERT_EQ(Seconds(1u << 31), errorRetryTimeout(ResourceStatus::ConnectionError, 50));
}

TEST(HttpRetry, RateLimit) {
    // Pre-set value from header
    ASSERT_EQ(Seconds(1), errorRetryTimeout(ResourceStatus::ServerError, 1, { util::now() + Seconds(1) }));

    // Default
    ASSERT_EQ(Seconds(5), errorRetryTimeout(ResourceStatus::RateLimitError, 1, {}));
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
