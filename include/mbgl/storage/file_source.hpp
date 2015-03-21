#ifndef MBGL_STORAGE_FILE_SOURCE
#define MBGL_STORAGE_FILE_SOURCE

#include "response.hpp"
#include "resource.hpp"

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/util.hpp>

#include <functional>
#include <thread>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class Request;
class Environment;

class FileSource : private util::noncopyable {
protected:
    MBGL_STORE_THREAD(tid)

public:
    virtual ~FileSource() = default;

    using Callback = std::function<void(const Response &)>;

    // These can be called from any thread. The callback will be invoked in the loop.
    // You can only cancel a request from the same thread it was created in.
    virtual Request *request(const Resource &resource, const std::thread::id& id, Callback callback) = 0;
    virtual void cancel(Request *request) = 0;

    // These can be called from any thread. The callback will be invoked in an arbitrary other thread.
    // You cannot cancel these requests.
    virtual void request(const Resource &resource, Callback callback) = 0;

    // This can be called from any thread. All requests with the environment pointer env should be
    // notified as errored. Note that this is /different/ from canceling requests; a canceled
    // request's callback is never called, while an aborted request's callback is called with
    // a error message.
    virtual void abort(uint32_t env) = 0;
};

}

#endif
