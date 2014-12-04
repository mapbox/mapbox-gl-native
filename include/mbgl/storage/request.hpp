#ifndef MBGL_STORAGE_REQUEST
#define MBGL_STORAGE_REQUEST

#include <mbgl/storage/request_callback.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/ptr.hpp>

#include <thread>
#include <forward_list>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class BaseRequest;

class Request {
private:
    Request(const Request &) = delete;
    Request(Request &&) = delete;
    Request& operator=(const Request &) = delete;
    Request& operator=(Request &&) = delete;

public:
    Request(const util::ptr<BaseRequest> &base);
    ~Request();

    void onload(CompletedCallback cb);
    void oncancel(AbortedCallback cb);
    void cancel();

private:
    const std::thread::id thread_id;
    util::ptr<BaseRequest> base;
    std::forward_list<Callback *> callbacks;
};

}

#endif