#include <mbgl/storage/http_request_base.hpp>

#include <mbgl/util/http_header.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

Seconds HTTPRequestBase::parseCacheControl(const char *value) {
    if (value) {
        const auto cacheControl = http::CacheControl::parse(value);
        if (cacheControl.maxAge) {
            return toSeconds(SystemClock::now()) + Seconds(*cacheControl.maxAge);
        }
    }

    return Seconds::zero();
}

} // namespace mbgl
