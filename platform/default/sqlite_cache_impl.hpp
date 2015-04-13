#ifndef MBGL_STORAGE_DEFAULT_SQLITE_CACHE_IMPL
#define MBGL_STORAGE_DEFAULT_SQLITE_CACHE_IMPL

#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/util/run_loop.hpp>

namespace mapbox {
namespace sqlite {
class Database;
class Statement;
}
}

namespace mbgl {

class SQLiteCache::Impl : public util::RunLoop {
    friend class util::Thread<SQLiteCache::Impl>;

public:
    Impl(const std::string &path = ":memory:");
    ~Impl();

public:
    void processGet(const Resource& resource, Callback callback);
    void processPut(const Resource& resource, std::shared_ptr<const Response> response);
    void processRefresh(const Resource& resource, int64_t expires);

private:
    void createDatabase();
    void createSchema();

private:
    const std::string path;
    std::unique_ptr<::mapbox::sqlite::Database> db;
    std::unique_ptr<::mapbox::sqlite::Statement> getStmt;
    std::unique_ptr<::mapbox::sqlite::Statement> putStmt;
    std::unique_ptr<::mapbox::sqlite::Statement> refreshStmt;
    bool schema = false;
};


}

#endif
