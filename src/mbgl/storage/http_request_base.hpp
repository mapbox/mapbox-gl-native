#ifndef MBGL_STORAGE_HTTP_REQUEST_BASE
#define MBGL_STORAGE_HTTP_REQUEST_BASE

#include <mbgl/storage/request_base.hpp>

namespace mbgl {

    class HTTPRequestBase : public RequestBase {
public:
    HTTPRequestBase(const Resource& resource_, Callback notify_)
        : RequestBase(resource_, notify_)
        , cancelled(false) {
    }

    virtual ~HTTPRequestBase() = default;
    virtual void cancel() override { cancelled = true; };

protected:
    static int64_t parseCacheControl(const char *value);

    bool cancelled;
};

} // namespace mbgl

#endif // MBGL_STORAGE_HTTP_REQUEST_BASE
