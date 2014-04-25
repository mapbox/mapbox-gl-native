#ifndef LLMR_PLATFORM_REQUEST
#define LLMR_PLATFORM_REQUEST

#include <string>
#include <functional>
#include <memory>

#include <llmr/util/noncopyable.hpp>

// Forward definition.
typedef struct uv_work_s uv_work_t;
typedef struct uv_check_s uv_check_t;
typedef struct uv_loop_s uv_loop_t;

namespace llmr {
namespace platform {

struct Response;

class Request : public std::enable_shared_from_this<Request>, private util::noncopyable {
public:
    Request(const std::string &url,
            std::function<void(Response *)> background_function,
            std::function<void()> foreground_callback,
            uv_loop_t *loop);
    ~Request();

public:
    static void work_callback(uv_work_t *work);
    static void after_work_callback(uv_work_t *work, int status);

public:
    const std::string url;
    const std::function<void(Response *)> background_function;
    const std::function<void()> foreground_callback;
    std::unique_ptr<Response> res;
    bool cancelled = false;

public:
    uv_check_t *check = nullptr;
    uv_loop_t *loop = nullptr;
};
}
}

#endif
