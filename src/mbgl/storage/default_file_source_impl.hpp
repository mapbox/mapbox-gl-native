#ifndef MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE_IMPL
#define MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE_IMPL

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/run_loop.hpp>

#include <set>
#include <unordered_map>

namespace mbgl {

class SharedRequestBase;

class DefaultFileSource::Impl : public util::RunLoop {
    friend class util::Thread<DefaultFileSource::Impl>;

public:
    Impl(FileCache *cache, const std::string &root = "");

public:
    void notify(SharedRequestBase *sharedRequest, const std::set<Request *> &observers,
                std::shared_ptr<const Response> response, FileCache::Hint hint);
    SharedRequestBase *find(const Resource &resource);

    void processAdd(Request* request);
    void processCancel(Request* request);
    void processResult(const Resource& resource, std::shared_ptr<const Response> response);
    void processAbort(const Environment& env);

public:
    const std::string assetRoot;

private:
    std::unordered_map<Resource, SharedRequestBase *, Resource::Hash> pending;
    FileCache *cache = nullptr;
};

}

#endif
