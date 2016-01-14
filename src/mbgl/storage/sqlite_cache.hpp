#ifndef MBGL_STORAGE_DEFAULT_SQLITE_CACHE
#define MBGL_STORAGE_DEFAULT_SQLITE_CACHE

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>

#include <functional>
#include <memory>
#include <string>

namespace mbgl {

struct Resource;
class Response;
class WorkRequest;

namespace util {
template <typename T> class Thread;
} // namespace util

class SQLiteCache : private util::noncopyable {
public:
    static std::shared_ptr<SQLiteCache> getShared(const std::string &path = ":memory:");

    SQLiteCache(const std::string &path = ":memory:");
    ~SQLiteCache();

    void setMaximumCacheSize(uint64_t size);
    void setMaximumCacheEntrySize(uint64_t size);

    using Callback = std::function<void(std::unique_ptr<Response>)>;

    std::unique_ptr<WorkRequest> get(const Resource&, Callback);
    void put(const Resource&, const Response&);

    class Impl;

private:
    const std::unique_ptr<util::Thread<Impl>> thread;
};

} // namespace mbgl

#endif
