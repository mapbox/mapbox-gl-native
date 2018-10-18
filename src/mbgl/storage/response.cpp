#include <mbgl/storage/response.hpp>

namespace mbgl {

Response::Response(const Response& res) {
    *this = res;
}

Response& Response::operator=(const Response& res) {
    error = res.error ? std::make_unique<Error>(*res.error) : nullptr;
    noContent = res.noContent;
    notModified = res.notModified;
    mustRevalidate = res.mustRevalidate;
    data = res.data;
    modified = res.modified;
    expires = res.expires;
    etag = res.etag;
    return *this;
}

Response::Error::Error(Reason reason_, std::string message_, optional<Timestamp> retryAfter_)
    : reason(reason_), message(std::move(message_)), retryAfter(std::move(retryAfter_)) {
}

} // namespace mbgl
