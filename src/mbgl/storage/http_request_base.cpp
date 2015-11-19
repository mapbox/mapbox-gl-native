#include <mbgl/storage/http_request_base.hpp>

#include <mbgl/util/http_header.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

int64_t HTTPRequestBase::parseCacheControl(const char *value) {
    if (value) {
        const auto cacheControl = http::CacheControl::parse(value);

        if (cacheControl.maxAge) {
            return std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::system_clock::now().time_since_epoch()).count() +
                   *cacheControl.maxAge;
        }
    }

    return 0;
}

}
