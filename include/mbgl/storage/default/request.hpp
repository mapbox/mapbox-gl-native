#ifndef MBGL_STORAGE_DEFAULT_REQUEST
#define MBGL_STORAGE_DEFAULT_REQUEST

#include <mbgl/storage/resource.hpp>

#include <mbgl/util/util.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <functional>
#include <memory>

typedef struct uv_async_s uv_async_t;
typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class Response;

class Request : private util::noncopyable {
    MBGL_STORE_THREAD(tid)

public:
    using Callback = std::function<void(const Response &)>;
    Request(const Resource &resource, uv_loop_t *loop, Callback callback);
    ~Request();

public:
    // May be called from any thread.
    void notify(const std::shared_ptr<const Response> &response);
    void destruct();

    // May be called only from the thread the Request was created in.
    void cancel();

private:
    static void notifyCallback(uv_async_t *async, int);
    static void cancelCallback(uv_async_t *async, int);

private:
    uv_async_t *notify_async = nullptr;
    uv_async_t *destruct_async = nullptr;
    Callback callback;
    std::shared_ptr<const Response> response;

public:
    const Resource resource;
};

}

#endif
