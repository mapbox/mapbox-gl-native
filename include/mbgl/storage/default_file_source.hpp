#ifndef MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE
#define MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/file_cache.hpp>

#include <set>
#include <unordered_map>
#include <thread>

namespace mapbox { namespace util { template<typename... Types> class variant; } }

namespace mbgl {

namespace util { template <typename T> class AsyncQueue; }

class SharedRequestBase;

class DefaultFileSource : public FileSource {
public:
    DefaultFileSource(FileCache *cache, const std::string &root = "");
    DefaultFileSource(FileCache *cache, uv_loop_t *loop, const std::string &root = "");
    ~DefaultFileSource();

    Request *request(const Resource &resource, uv_loop_t *loop, Callback callback);
    void cancel(Request *request);
    void request(const Resource &resource, Callback callback);

    enum class CacheHint : uint8_t { Full, Refresh, No };
    void notify(SharedRequestBase *sharedRequest, const std::set<Request *> &observers,
                std::shared_ptr<const Response> response, FileCache::Hint hint);

public:
    const std::string assetRoot;

private:
    struct ActionDispatcher;
    struct AddRequestAction;
    struct RemoveRequestAction;
    struct ResultAction;
    struct StopAction;
    using Action =
        mapbox::util::variant<AddRequestAction, RemoveRequestAction, ResultAction, StopAction>;
    using Queue = util::AsyncQueue<Action>;

    void process(AddRequestAction &action);
    void process(RemoveRequestAction &action);
    void process(ResultAction &action);
    void process(StopAction &action);

    SharedRequestBase *find(const Resource &resource);

    std::unordered_map<Resource, SharedRequestBase *, Resource::Hash> pending;

    uv_loop_t *loop = nullptr;
    FileCache *cache = nullptr;
    Queue *queue = nullptr;
    std::thread thread;
};

}

#endif
