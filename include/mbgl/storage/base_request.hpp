#ifndef MBGL_STORAGE_BASE_REQUEST
#define MBGL_STORAGE_BASE_REQUEST

#include <string>
#include <forward_list>
#include <memory>
#include <functional>

typedef struct uv_loop_s uv_loop_t;
typedef struct uv_async_s uv_async_t;

namespace mbgl {

class Response;
class Request;
using Callback = std::function<void(const Response &)>;


class BaseRequest {
private:
    // Make noncopyable and immovable
    BaseRequest(const BaseRequest &) = delete;
    BaseRequest(BaseRequest &&) = delete;
    BaseRequest& operator=(const BaseRequest &) = delete;
    BaseRequest& operator=(BaseRequest &&) = delete;

public:
    BaseRequest();
    virtual ~BaseRequest();

    Callback *add(Callback &&callback, const std::shared_ptr<BaseRequest> &request);
    void remove(Callback *callback);
    void notify();

public:
    const unsigned long thread_id;
    std::unique_ptr<Response> response;

private:
    std::shared_ptr<BaseRequest> self;
    std::forward_list<std::unique_ptr<Callback>> callbacks;
};

}

#endif
