#pragma once

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

    // This is set to true for 204 Not Modified responses, and, for backward compatibility,
    // for 404 Not Found responses for tiles.
    bool noContent = false;

    // This is set to true for 304 Not Modified responses.
    bool notModified = false;

    // This is set to true when the server requested that no expired resources be used by
    // specifying "Cache-Control: must-revalidate".
    bool mustRevalidate = false;

    // The actual data of the response. Present only for non-error, non-notModified responses.
    std::shared_ptr<const std::string> data;

    optional<Timestamp> modified;
    optional<Timestamp> expires;
    optional<std::string> etag;

    bool isFresh() const {
        return expires ? *expires > util::now() : !error;
    }

    // Indicates whether we are allowed to use this response according to HTTP caching rules.
    // It may or may not be stale.
    bool isUsable() const {
        return !mustRevalidate || (expires && *expires > util::now());
    }
};

class Response::Error {
public:
    enum class Reason : uint8_t {
        Success = 1,
        NotFound = 2,
        Server = 3,
        Connection = 4,
        RateLimit = 5,
        Other = 6,
    } reason = Reason::Other;

    // An error message from the request handler, e.g. a server message or a system message
    // informing the user about the reason for the failure.
    std::string message;

    optional<Timestamp> retryAfter;

public:
    Error(Reason, std::string = "", optional<Timestamp> = {});
};

std::ostream& operator<<(std::ostream&, Response::Error::Reason);

} // namespace mbgl
