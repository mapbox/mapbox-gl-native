#ifndef MBGL_STORAGE_REQUEST
#define MBGL_STORAGE_REQUEST

#include <mbgl/storage/response.hpp>

#include <memory>
#include <functional>
#include <forward_list>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class BaseRequest;
using Callback = std::function<void(const Response &)>;

class Request {
private:
    Request(const Request &) = delete;
    Request(Request &&) = delete;
    Request& operator=(const Request &) = delete;
    Request& operator=(Request &&) = delete;

public:
    Request(const std::shared_ptr<BaseRequest> &base);
    ~Request();

    void onload(Callback cb);
    void cancel();

private:
    const unsigned long thread_id;
    std::shared_ptr<BaseRequest> base;
    std::forward_list<Callback *> callbacks;
};

}

#endif