#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/enum.hpp>

#include <iostream>
#include <cassert>

namespace mbgl {

MBGL_DEFINE_ENUM(ResourceStatus, {
    { ResourceStatus::Success, "Success" },
    { ResourceStatus::NotFoundError, "NotFoundError" },
    { ResourceStatus::ServerError, "ServerError" },
    { ResourceStatus::ConnectionError, "ConnectionError" },
    { ResourceStatus::RateLimitError, "RateLimitError" },
    { ResourceStatus::OtherError, "OtherError" },
});

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

} // namespace mbgl
