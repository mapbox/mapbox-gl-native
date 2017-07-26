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
    data = res.data;
    modified = res.modified;
    expires = res.expires;
    etag = res.etag;
    return *this;
}

Response::Error::Error(ResourceStatus reason_, std::string message_, optional<Timestamp> retryAfter_)
    : status(reason_), message(std::move(message_)), retryAfter(std::move(retryAfter_)) {
}

std::ostream& operator<<(std::ostream& os, ResourceStatus r) {
    switch (r) {
    case ResourceStatus::Success:
        return os << "ResourceStatus::Success";
    case ResourceStatus::NotFoundError:
        return os << "ResourceStatus::NotFoundError";
    case ResourceStatus::ServerError:
        return os << "ResourceStatus::ServerError";
    case ResourceStatus::ConnectionError:
        return os << "ResourceStatus::ConnectionError";
    case ResourceStatus::RateLimitError:
        return os << "ResourceStatus::RateLimitError";
    case ResourceStatus::OtherError:
        return os << "ResourceStatus::OtherError";
    }

    // The above switch is exhaustive, but placate GCC nonetheless:
    assert(false);
    return os;
}

} // namespace mbgl
