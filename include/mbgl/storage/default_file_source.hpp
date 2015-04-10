#ifndef MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE
#define MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/file_cache.hpp>
#include <mbgl/util/run_loop.hpp>

#include <set>
#include <unordered_map>

namespace mbgl {

class SharedRequestBase;

class DefaultFileSource : public FileSource, protected util::RunLoop {
    friend class util::Thread<DefaultFileSource>;
    friend class SharedRequestBase;

private:
    DefaultFileSource(FileCache *cache, const std::string &root = "");
    ~DefaultFileSource() override;

public:
    // FileSource API
    Request *request(const Resource &resource, uv_loop_t *loop, const Environment &env,
                     Callback callback) override;
    void cancel(Request *request) override;
    void request(const Resource &resource, const Environment &env, Callback callback) override;

    void abort(const Environment &env) override;

private:
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
