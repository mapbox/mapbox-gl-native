#ifndef MBGL_STORAGE_DEFAULT_HTTP_REQUEST
#define MBGL_STORAGE_DEFAULT_HTTP_REQUEST

#include "shared_request_base.hpp"

namespace mbgl {

class HTTPRequest : public SharedRequestBase {
public:
    HTTPRequest(DefaultFileSource::Impl &source, const Resource &resource);

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
