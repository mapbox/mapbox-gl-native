#ifndef MBGL_STORAGE_RESPONSE
#define MBGL_STORAGE_RESPONSE

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

#include <string>
#include <memory>

namespace mbgl {

class Response {
public:
    Response() = default;
    Response(const Response&);
    Response& operator=(const Response&);

public:
    class Error;
    // When this object is empty, the response was successful.
    std::unique_ptr<const Error> error;

    // This is set to true for 304 Not Modified responses.
    bool notModified = false;

    // The actual data of the response. Present only for non-error, non-notModified responses.
    std::shared_ptr<const std::string> data;

    optional<SystemTimePoint> modified;
    optional<SystemTimePoint> expires;
    optional<std::string> etag;
};

class Response::Error {
public:
    enum class Reason : uint8_t {
        Success = 1,
        NotFound = 2,
        Server = 3,
        Connection = 4,
        Other = 6,
    } reason = Reason::Other;

    // An error message from the request handler, e.g. a server message or a system message
    // informing the user about the reason for the failure.
    std::string message;

public:
    Error(Reason, const std::string& = "");
};

} // namespace mbgl

#endif
