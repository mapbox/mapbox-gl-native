#ifndef MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE_IMPL
#define MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE_IMPL

#include <mbgl/storage/default_file_source.hpp>

#include <set>
#include <unordered_map>

namespace mbgl {

class RequestBase;

struct DefaultFileRequest {
    const Resource resource;
    uv_loop_t* loop;
    std::set<Request*> observers;
    RequestBase* request = nullptr;

    DefaultFileRequest(const Resource& resource_, uv_loop_t* loop_)
        : resource(resource_), loop(loop_) {}
};

class DefaultFileSource::Impl {
public:
    Impl(FileCache*, const std::string& = "");

    void add(Request*, uv_loop_t*);
    void cancel(Request*);
    void abort(const Environment&);

private:
    DefaultFileRequest* find(const Resource&);

    void startCacheRequest(const Resource&);
    void startRealRequest(const Resource&, std::shared_ptr<const Response> = nullptr);
    void notify(DefaultFileRequest*, std::shared_ptr<const Response>, FileCache::Hint);

    std::unordered_map<Resource, DefaultFileRequest, Resource::Hash> pending;
    FileCache* cache = nullptr;
    const std::string assetRoot;
};

}

#endif
