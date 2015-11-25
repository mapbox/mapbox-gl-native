#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

Response::Response(const Response& res) {
    *this = res;
}

Response& Response::operator=(const Response& res) {
    error = res.error ? std::make_unique<Error>(*res.error) : nullptr;
    stale = res.stale;
    data = res.data;
    modified = res.modified;
    expires = res.expires;
    etag = res.etag;
    return *this;
}

bool Response::isExpired() const {
    // Note: expires == 0 also counts as expired!
    return SystemTimePoint(expires) <= SystemClock::now();
}

Response::Error::Error(Reason reason_, const std::string& message_)
    : reason(reason_), message(message_) {
}

} // namespace mbgl
