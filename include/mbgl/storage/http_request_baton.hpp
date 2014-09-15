#ifndef MBGL_STORAGE_HTTP_REQUEST_BATON
#define MBGL_STORAGE_HTTP_REQUEST_BATON

#include <mbgl/storage/response.hpp>

#include <string>

typedef struct uv_async_s uv_async_t;

namespace mbgl {

class HTTPRequest;

struct HTTPRequestBaton {
    HTTPRequest *request = nullptr;
    std::string path;
    uv_async_t *async = nullptr;
    std::unique_ptr<Response> response;
    void *ptr = nullptr;
    bool not_modified = false;

    void start();
    void cancel();
};

}

#endif
