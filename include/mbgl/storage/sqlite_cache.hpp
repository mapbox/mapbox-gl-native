#ifndef MBGL_STORAGE_DEFAULT_SQLITE_CACHE
#define MBGL_STORAGE_DEFAULT_SQLITE_CACHE

#include <mbgl/storage/file_cache.hpp>

#include <string>

namespace mbgl {

namespace util {
template <typename T> class Thread;
} // namespace util

class SQLiteCache : public FileCache {
public:
    SQLiteCache(const std::string &path = ":memory:");
    ~SQLiteCache() override;

    void setMaximumCacheSize(uint64_t size);
    void setMaximumCacheEntrySize(uint64_t size);

    // FileCache API
    std::unique_ptr<WorkRequest> get(const std::string& url, Callback callback) override;
    void put(const std::string& url, std::shared_ptr<const Response> response, Hint hint) override;

    class Impl;

private:
    const std::unique_ptr<util::Thread<Impl>> thread;
};

class SharedSQLiteCache : util::noncopyable {
public:
    static std::shared_ptr<SQLiteCache> get(const std::string &path = ":memory:");

private:
    SharedSQLiteCache() {}

    static std::weak_ptr<SQLiteCache> masterPtr;
};

} // namespace mbgl

#endif
