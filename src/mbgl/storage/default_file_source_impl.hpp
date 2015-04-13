#ifndef MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE_IMPL
#define MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE_IMPL

#include <mbgl/storage/default_file_source.hpp>

#include <set>
#include <unordered_map>

namespace mbgl {

class SharedRequestBase;

class DefaultFileSource::Impl {
public:
    Impl(FileCache *cache, const std::string &root = "");

    void notify(SharedRequestBase *sharedRequest, const std::set<Request *> &observers,
                std::shared_ptr<const Response> response, FileCache::Hint hint);
    SharedRequestBase *find(const Resource &resource);

    void processAdd(Request* request, uv_loop_t* loop);
    void processCancel(Request* request);
    void processAbort(const Environment& env);

    const std::string assetRoot;

private:
    void processResult(const Resource& resource, std::shared_ptr<const Response> response, uv_loop_t* loop);

    std::unordered_map<Resource, SharedRequestBase *, Resource::Hash> pending;
    FileCache *cache = nullptr;
};

}

#endif
