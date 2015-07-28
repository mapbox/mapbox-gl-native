#ifndef MBGL_STORAGE_HTTP_REQUEST_BASE
#define MBGL_STORAGE_HTTP_REQUEST_BASE

namespace mbgl {

enum class ResponseStatus : uint8_t {
    // This error probably won't be resolved by retrying anytime soon. We are giving up.
    PermanentError,

    // This error might be resolved by waiting some time (e.g. server issues).
    // We are going to do an exponential back-off and will try again in a few seconds.
    TemporaryError,

    // This error was caused by a temporary error and it is likely that it will be resolved
    // immediately. We are going to try again right away. This is like the TemporaryError, except
    // that we will not perform exponential back-off.
    SingularError,

    // This error might be resolved once the network reachability status changes.
    // We are going to watch the network status for changes and will retry as soon as the
    // operating system notifies us of a network status change.
    ConnectionError,

    // The request was canceled mid-way.
    Canceled,

    // The request returned data successfully. We retrieved and decoded the data successfully.
    Successful,

    // The request confirmed that the data wasn't changed. We already have the data.
    NotModified,
};

} // namespace mbgl

#endif // MBGL_STORAGE_HTTP_REQUEST_BASE
