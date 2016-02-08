#ifndef MBGL_STORAGE_HTTP_REQUEST_BASE
#define MBGL_STORAGE_HTTP_REQUEST_BASE

#include <mbgl/storage/response.hpp>
#include <mbgl/storage/resource.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

#include <functional>

namespace mbgl {

class Response;

class HTTPRequestBase : private util::noncopyable {
public:
    using Callback = std::function<void (Response)>;

    HTTPRequestBase(const Resource& resource_, Callback notify_)
        : resource(resource_)
        , notify(std::move(notify_))
        , cancelled(false) {
    }

    virtual ~HTTPRequestBase() = default;
    virtual void cancel() { cancelled = true; };

protected:
    static optional<SystemTimePoint> parseCacheControl(const char *value);

    Resource resource;
    Callback notify;
    bool cancelled;
};

} // namespace mbgl

#endif // MBGL_STORAGE_HTTP_REQUEST_BASE
