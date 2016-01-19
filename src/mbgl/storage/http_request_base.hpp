#ifndef MBGL_STORAGE_HTTP_REQUEST_BASE
#define MBGL_STORAGE_HTTP_REQUEST_BASE

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

#include <memory>
#include <functional>
#include <utility>
#include <string>

namespace mbgl {

class Response;

class HTTPRequestBase : private util::noncopyable {
public:
    using Callback = std::function<void (std::shared_ptr<const Response> response)>;

    HTTPRequestBase(const std::string& url_, Callback notify_)
        : url(url_)
        , notify(std::move(notify_))
        , cancelled(false) {
    }

    virtual ~HTTPRequestBase() = default;
    virtual void cancel() { cancelled = true; };

protected:
    static optional<SystemTimePoint> parseCacheControl(const char *value);

    std::string url;
    Callback notify;
    bool cancelled;
};

} // namespace mbgl

#endif // MBGL_STORAGE_HTTP_REQUEST_BASE
