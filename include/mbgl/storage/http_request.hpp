#ifndef MBGL_STORAGE_HTTP_REQUEST
#define MBGL_STORAGE_HTTP_REQUEST

#include <mbgl/storage/resource_type.hpp>
#include <mbgl/storage/base_request.hpp>
#include <mbgl/storage/http_request_baton.hpp>

#include <string>
#include <memory>
#include <cassert>
#include <thread>

typedef struct uv_loop_s uv_loop_t;
typedef struct uv_timer_s uv_timer_t;

namespace mbgl {

struct CacheRequestBaton;
struct HTTPRequestBaton;
struct CacheEntry;
class SQLiteStore;

class HTTPRequest : public BaseRequest {
public:
    HTTPRequest(ResourceType type, const std::string &path, uv_loop_t *loop, util::ptr<SQLiteStore> store);
    ~HTTPRequest();

    void cancel();
    void retryImmediately();

private:
    void startCacheRequest();
    void handleCacheResponse(std::unique_ptr<Response> &&response);
    void startHTTPRequest(std::unique_ptr<Response> &&res);
    void handleHTTPResponse(HTTPResponseType responseType, std::unique_ptr<Response> &&response);

    void retryHTTPRequest(std::unique_ptr<Response> &&res, uint64_t timeout);

    void removeCacheBaton();
    void removeHTTPBaton();
    void removeBackoffTimer();

private:
    const std::thread::id thread_id;
    uv_loop_t *const loop;
    CacheRequestBaton *cache_baton = nullptr;
    util::ptr<HTTPRequestBaton> http_baton;
    uv_timer_t *backoff_timer = nullptr;
    util::ptr<SQLiteStore> store;
    const ResourceType type;
    uint8_t attempts = 0;

    friend struct HTTPRequestBaton;
};

}

#endif