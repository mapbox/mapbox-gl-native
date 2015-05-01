#ifndef MBGL_STORAGE_REQUEST
#define MBGL_STORAGE_REQUEST

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
    virtual void retry() {};

protected:
    Resource resource;
    Callback notify;
};

}

#endif
