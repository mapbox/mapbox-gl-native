#ifndef MBGL_STORAGE_REQUEST_BASE
#define MBGL_STORAGE_REQUEST_BASE

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/storage/file_cache.hpp>
#include <mbgl/storage/resource.hpp>

#include <memory>
#include <functional>

namespace mbgl {

class Response;

class RequestBase : private util::noncopyable {
public:
    using Callback = std::function<void (std::shared_ptr<const Response> response, FileCache::Hint hint)>;

    RequestBase(const Resource& resource_, Callback notify_)
        : resource(resource_)
        , notify(notify_) {
    }

    virtual ~RequestBase() = default;
    virtual void cancel() = 0;

protected:
    Resource resource;
    Callback notify;
};

} // namespace mbgl

#endif // MBGL_STORAGE_REQUEST_BASE
