#ifndef MBGL_STORAGE_REQUEST
#define MBGL_STORAGE_REQUEST

#include <mbgl/storage/response.hpp>
#include <mbgl/util/ptr.hpp>

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
    Request(const util::ptr<BaseRequest> &base);
    ~Request();

    void onload(Callback cb);
    void cancel();

private:
    const unsigned long thread_id;
    util::ptr<BaseRequest> base;
    std::forward_list<Callback *> callbacks;
};

}

#endif