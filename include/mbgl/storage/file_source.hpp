#ifndef MBGL_STORAGE_FILE_SOURCE
#define MBGL_STORAGE_FILE_SOURCE

#include "response.hpp"
#include "resource.hpp"

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/util.hpp>

#include <functional>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class Request;

class FileSource : private util::noncopyable {
protected:
    MBGL_STORE_THREAD(tid)

public:
    virtual ~FileSource() = default;

    using Callback = std::function<void(const Response &)>;

    // These can be called from any thread. The callback will be invoked in the loop.
    // You can only cancel a request from the same thread it was created in.
    virtual Request* request(const Resource&, uv_loop_t*, Callback) = 0;
    virtual void cancel(Request*) = 0;
};

}

#endif
