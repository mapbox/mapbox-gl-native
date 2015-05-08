#ifndef MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE_IMPL
#define MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE_IMPL

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/asset_context.hpp>
#include <mbgl/storage/http_context.hpp>

#include <set>
#include <unordered_map>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class RequestBase;

struct DefaultFileRequest : private util::noncopyable {
    const Resource resource;
    std::set<Request*> observers;
    CacheRequest cacheRequest;
    RequestBase* realRequest = nullptr;

    inline DefaultFileRequest(const Resource& resource_)
        : resource(resource_) {}
};

class DefaultFileSource::Impl {
public:
    Impl(uv_loop_t*, FileCache*, const std::string& = "");

    void add(Request*);
    void cancel(Request*);

private:
    DefaultFileRequest* find(const Resource&);

    void startCacheRequest(DefaultFileRequest*);
    void startRealRequest(DefaultFileRequest*, std::shared_ptr<const Response> = nullptr);
    void notify(DefaultFileRequest*, std::shared_ptr<const Response>, FileCache::Hint);

    std::unordered_map<Resource, std::unique_ptr<DefaultFileRequest>, Resource::Hash> pending;
    uv_loop_t* loop = nullptr;
    FileCache* cache = nullptr;
    const std::string assetRoot;
    std::unique_ptr<AssetContext> assetContext;
    std::unique_ptr<HTTPContext> httpContext;
};

}

#endif
