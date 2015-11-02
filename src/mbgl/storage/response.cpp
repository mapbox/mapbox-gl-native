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
    const int64_t now =
        std::chrono::duration_cast<std::chrono::seconds>(SystemClock::now().time_since_epoch())
            .count();
    // Note: expires == 0 also counts as expired!
    return expires <= now;
}

Response::Error::Error(Reason reason_, const std::string& message_)
    : reason(reason_), message(message_) {
}

} // namespace mbgl
