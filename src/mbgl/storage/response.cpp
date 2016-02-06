#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>

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

Response::Error::Error(Reason reason_, const std::string& message_)
    : reason(reason_), message(message_) {
}

} // namespace mbgl
