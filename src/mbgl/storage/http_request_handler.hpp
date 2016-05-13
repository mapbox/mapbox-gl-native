#ifndef MBGL_STORAGE_HTTP_REQUEST_HANDLER
#define MBGL_STORAGE_HTTP_REQUEST_HANDLER

#include <mbgl/storage/response.hpp>
#include <mbgl/storage/resource.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/async_request.hpp>

#include <functional>
#include <memory>

namespace mbgl {
namespace storage {

class HTTPRequestHandler : private util::noncopyable {
public:
    HTTPRequestHandler();
    ~HTTPRequestHandler();

    using Callback = std::function<void (Response)>;
    std::unique_ptr<AsyncRequest> request(const Resource&, Callback);

    static uint32_t maximumConcurrentRequests();

    class Impl;

private:
    std::unique_ptr<Impl> impl;
};

} // namespace storage
} // namespace mbgl

#endif // MBGL_STORAGE_HTTP_REQUEST_HANDLER
