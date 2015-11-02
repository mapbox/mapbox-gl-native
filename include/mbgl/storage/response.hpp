#ifndef MBGL_STORAGE_RESPONSE
#define MBGL_STORAGE_RESPONSE

#include <string>
#include <memory>

namespace mbgl {

class Response {
public:
    Response() = default;
    Response(const Response&);
    Response& operator=(const Response&);

    bool isExpired() const;

public:
    class Error;
    // When this object is empty, the response was successful.
    std::unique_ptr<const Error> error;

    // Stale responses are fetched from cache and are expired.
    bool stale = false;

    // The actual data of the response. This is guaranteed to never be empty.
    std::shared_ptr<const std::string> data;

    int64_t modified = 0;
    int64_t expires = 0;
    std::string etag;
};

class Response::Error {
public:
    enum class Reason : uint8_t {
        // Success = 1, // Reserve 1 for Success.
        NotFound = 2,
        Server = 3,
        Connection = 4,
        Canceled = 5,
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
