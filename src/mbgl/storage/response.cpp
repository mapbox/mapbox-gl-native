#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>

#include <iostream>
#include <cassert>

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

std::ostream& operator<<(std::ostream& os, Response::Error::Reason r) {
    switch (r) {
    case Response::Error::Reason::Success:
        return os << "Response::Error::Reason::Success";
    case Response::Error::Reason::NotFound:
        return os << "Response::Error::Reason::NotFound";
    case Response::Error::Reason::Server:
        return os << "Response::Error::Reason::Server";
    case Response::Error::Reason::Connection:
        return os << "Response::Error::Reason::Connection";
    case Response::Error::Reason::RateLimit:
        return os << "Response::Error::Reason::RateLimit";
    case Response::Error::Reason::Other:
        return os << "Response::Error::Reason::Other";
    }

    // The above switch is exhaustive, but placate GCC nonetheless:
    assert(false);
    return os;
}

} // namespace mbgl
