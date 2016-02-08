#include <mbgl/storage/http_request_base.hpp>

#include <mbgl/util/http_header.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

optional<SystemTimePoint> HTTPRequestBase::parseCacheControl(const char *value) {
    if (!value) {
        return {};
    }

    const auto cacheControl = http::CacheControl::parse(value);
    if (!cacheControl.maxAge) {
        return {};
    }

    // Round trip through time_t to truncate fractional seconds.
    return SystemClock::from_time_t(SystemClock::to_time_t(
        SystemClock::now() + std::chrono::seconds(*cacheControl.maxAge)));
}

} // namespace mbgl
