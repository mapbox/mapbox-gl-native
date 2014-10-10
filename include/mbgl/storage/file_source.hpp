#ifndef MBGL_STORAGE_FILE_SOURCE
#define MBGL_STORAGE_FILE_SOURCE

#include <mbgl/storage/resource_type.hpp>
#include <mbgl/storage/request.hpp>

#include <string>
#include <unordered_map>
#include <functional>

typedef struct uv_loop_s uv_loop_t;
typedef struct uv_messenger_s uv_messenger_t;

namespace mbgl {

class BaseRequest;
class SQLiteStore;

class FileSource {
private:
    FileSource(const FileSource &) = delete;
    FileSource(FileSource &&) = delete;
    FileSource& operator=(const FileSource &) = delete;
    FileSource& operator=(FileSource &&) = delete;

public:
    FileSource(uv_loop_t *loop, const std::string &path);
    ~FileSource();

public:
    // Stores and retrieves the base path/URL for relative requests
    void setBase(const std::string &value);
    const std::string &getBase() const;

    std::unique_ptr<Request> request(ResourceType type, const std::string &url);

    void prepare(std::function<void()> fn);

    void retryAllPending();

private:
    const unsigned long thread_id;

    // Stores a URL that is used as a base for loading resources with relative path.
    std::string base;

    std::unordered_map<std::string, std::weak_ptr<BaseRequest>> pending;
    util::ptr<SQLiteStore> store;
    uv_loop_t *loop = nullptr;
    uv_messenger_t *queue = nullptr;
};

}

#endif
