#ifndef MBGL_STORAGE_HTTP_REQUEST_BATON
#define MBGL_STORAGE_HTTP_REQUEST_BATON

#include <mbgl/storage/response.hpp>
#include <mbgl/util/ptr.hpp>

#include <string>

typedef struct uv_async_s uv_async_t;

namespace mbgl {

class HTTPRequest;

enum class HTTPResponseType : int8_t {
    // This error probably won't be resolved by retrying anytime soon. We are giving up.
    PermanentError = -5,

    // This error might be resolved by waiting some time (e.g. server issues).
    // We are going to do an exponential back-off and will try again in a few seconds.
    TemporaryError = -4,

    // This error was caused by a temporary error and it is likely that it will be resolved
    // immediately. We are going to try again right away. This is like the TemporaryError, except
    // that we will not perform exponential back-off.
    SingularError = -3,

    // This error might be resolved once the network reachability status changes.
    // We are going to watch the network status for changes and will retry as soon as the operating
    // system notifies us of a network status change.
    ConnectionError = -2,

    // The request was canceled programatically.
    Canceled = -1,

    // The request is still in progress.
    Unknown = 0,

    // The request returned data successfully. We retrieved and decoded the data successfully.
    Successful = 1,

    // The request confirmed that the data wasn't changed. We already have the data.
    NotModified = 2,
};

struct HTTPRequestBaton {
    HTTPRequestBaton(const std::string &path);
    ~HTTPRequestBaton();

    const unsigned long threadId;
    const std::string path;

    HTTPRequest *request = nullptr;
    uv_async_t *async = nullptr;

    HTTPResponseType type = HTTPResponseType::Unknown;
    std::unique_ptr<Response> response;

    // Implementation specific use.
    void *ptr = nullptr;

    // IMPLEMENT THESE 3 PLATFORM SPECIFIC FUNCTIONS:

    // Begin the HTTP request. Platform-specific implementation.
    static void start(const util::ptr<HTTPRequestBaton> &ptr);

    // This will be called to stop/cancel the HTTP request (if possible). Platform-specific implementation.
    static void stop(const util::ptr<HTTPRequestBaton> &ptr);
};

}

#endif
