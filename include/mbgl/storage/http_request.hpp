#ifndef MBGL_STORAGE_HTTP_REQUEST
#define MBGL_STORAGE_HTTP_REQUEST

#include <mbgl/storage/resource_type.hpp>
#include <mbgl/storage/base_request.hpp>

#include <string>
#include <memory>
#include <cassert>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

struct CacheRequestBaton;
struct HTTPRequestBaton;
struct CacheEntry;
class SQLiteStore;

class HTTPRequest : public BaseRequest {
public:
    HTTPRequest(ResourceType type, const std::string &path, uv_loop_t *loop, std::shared_ptr<SQLiteStore> store);
    ~HTTPRequest();

private:
    void loadedCacheEntry(std::unique_ptr<Response> &&response);

private:
    const unsigned long thread_id;
    CacheRequestBaton *cache_baton = nullptr;
    HTTPRequestBaton *http_baton = nullptr;
    std::shared_ptr<SQLiteStore> store;
    const ResourceType type;
};

}

#endif