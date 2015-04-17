#ifndef MBGL_STORAGE_DEFAULT_HTTP_REQUEST
#define MBGL_STORAGE_DEFAULT_HTTP_REQUEST

#include "request_base.hpp"

namespace mbgl {

struct Resource;

class HTTPRequest : public RequestBase {
public:
    HTTPRequest(const Resource&, Callback);

    void start(uv_loop_t *loop, std::shared_ptr<const Response> response = nullptr);
    void cancel();

    void retryImmediately();

private:
    ~HTTPRequest();
    void *ptr = nullptr;

    friend class HTTPRequestImpl;
};

}

#endif
