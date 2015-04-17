#ifndef MBGL_STORAGE_DEFAULT_HTTP_REQUEST
#define MBGL_STORAGE_DEFAULT_HTTP_REQUEST

#include "request_base.hpp"

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class HTTPRequestImpl;

class HTTPRequest : public RequestBase {
public:
    HTTPRequest(const Resource&, Callback, uv_loop_t*, std::shared_ptr<const Response> = nullptr);

    void cancel() override;
    void retryImmediately();

private:
    ~HTTPRequest();

    HTTPRequestImpl* impl;
    friend class HTTPRequestImpl;
};

}

#endif
