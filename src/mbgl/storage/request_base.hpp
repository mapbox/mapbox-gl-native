#ifndef MBGL_STORAGE_REQUEST
#define MBGL_STORAGE_REQUEST

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/storage/file_cache.hpp>

#include <memory>
#include <functional>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

struct Resource;
class Response;

class RequestBase : private util::noncopyable {
public:
    using Callback = std::function<void (std::shared_ptr<const Response> response, FileCache::Hint hint)>;

    RequestBase(const Resource& resource_, Callback notify_)
        : notify(notify_)
        , resource(resource_) {
    }

    virtual ~RequestBase() = default;

    virtual void start(uv_loop_t *loop, std::shared_ptr<const Response> response = nullptr) = 0;
    virtual void cancel() = 0;

    Callback notify;

protected:
    const Resource& resource;
};

}

#endif
