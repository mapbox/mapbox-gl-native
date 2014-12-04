#ifndef MBGL_STORAGE_CACHING_HTTP_FILE_SOURCE
#define MBGL_STORAGE_CACHING_HTTP_FILE_SOURCE

#include "file_source.hpp"

#include <thread>
#include <unordered_map>

typedef struct uv_messenger_s uv_messenger_t;

namespace mbgl {

class BaseRequest;
class SQLiteStore;

class CachingHTTPFileSource : public FileSource {
public:
    CachingHTTPFileSource(const std::string &path_);
    virtual ~CachingHTTPFileSource();

    // Stores and checks the libuv loop for requests
    void setLoop(uv_loop_t*);
    bool hasLoop();

    // Stores and retrieves the base path/URL for relative requests
    void setBase(const std::string &value);
    const std::string &getBase() const;

    std::unique_ptr<Request> request(ResourceType type, const std::string &url);

    void prepare(std::function<void()> fn);

    void retryAllPending();

private:
    std::thread::id thread_id;

    // Path to the cache database.
    std::string path;

    // Stores a URL that is used as a base for loading resources with relative path.
    std::string base;

    std::unordered_map<std::string, std::weak_ptr<BaseRequest>> pending;
    util::ptr<SQLiteStore> store;
    uv_loop_t *loop = nullptr;
    uv_messenger_t *queue = nullptr;
};

}

#endif
