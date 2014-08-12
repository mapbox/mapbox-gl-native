#ifndef MBGL_PLATFORM_REQUEST
#define MBGL_PLATFORM_REQUEST

#include <string>
#include <functional>
#include <memory>
#include <atomic>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/uv.hpp>

namespace mbgl {
namespace platform {

struct Response;

class Request : public std::enable_shared_from_this<Request>, private util::noncopyable {
public:
    Request(const std::string &url,
            std::function<void(Response *)> callback,
            std::shared_ptr<uv::loop> loop);
    ~Request();

    void complete();

private:
    static void complete(uv_async_t *async);

public:
    const std::string url;
    std::unique_ptr<Response> res;
    std::atomic<bool> cancelled;

public:
    uv_async_t *async = nullptr;
    std::shared_ptr<uv::loop> loop;
};
}
}

#endif
