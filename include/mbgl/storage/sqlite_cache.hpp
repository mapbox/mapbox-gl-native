#ifndef MBGL_STORAGE_DEFAULT_SQLITE_CACHE
#define MBGL_STORAGE_DEFAULT_SQLITE_CACHE

#include <mbgl/storage/file_cache.hpp>

#include <string>

namespace mbgl {

namespace util {
template <typename T> class Thread;
}

class SQLiteCache : public FileCache {
public:
    SQLiteCache(const std::string &path = ":memory:");
    ~SQLiteCache() override;

    // FileCache API
    std::unique_ptr<WorkRequest> get(const Resource &resource, Callback callback) override;
    void put(const Resource &resource, std::shared_ptr<const Response> response, Hint hint) override;

    class Impl;

private:
    const std::unique_ptr<util::Thread<Impl>> thread;
};

}

#endif
