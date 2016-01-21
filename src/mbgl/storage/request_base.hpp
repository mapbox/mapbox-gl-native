#ifndef MBGL_STORAGE_REQUEST_BASE
#define MBGL_STORAGE_REQUEST_BASE

#include <mbgl/util/noncopyable.hpp>

#include <memory>
#include <functional>
#include <utility>
#include <string>

namespace mbgl {

class Response;

class RequestBase : private util::noncopyable {
public:
    using Callback = std::function<void (std::shared_ptr<const Response> response)>;

    RequestBase(const std::string& url_, Callback notify_)
        : url(url_)
        , notify(std::move(notify_)) {
    }

    virtual ~RequestBase() = default;
    virtual void cancel() = 0;

protected:
    std::string url;
    Callback notify;
};

} // namespace mbgl

#endif // MBGL_STORAGE_REQUEST_BASE
