#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/chrono.hpp>

#include <string>

namespace mbgl {
namespace http {

class CacheControl {
public:
    static CacheControl parse(const std::string&);

    optional<uint64_t> maxAge;
    bool mustRevalidate = false;

    optional<Timestamp> toTimePoint() const;
};

optional<Timestamp> parseRetryHeaders(const optional<std::string>& retryAfter,
                                      const optional<std::string>& xRateLimitReset);

} // namespace http
} // namespace mbgl
