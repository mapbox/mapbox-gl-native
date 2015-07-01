#ifndef MBGL_STORAGE_DEFAULT_SQLITE_CACHE_IMPL
#define MBGL_STORAGE_DEFAULT_SQLITE_CACHE_IMPL

#include <mbgl/storage/sqlite_cache.hpp>

namespace mapbox {
namespace sqlite {
class Database;
class Statement;
}
}

namespace mbgl {

class SQLiteCache::Impl {
public:
    Impl(const std::string &path = ":memory:");
    ~Impl();

    void get(const Resource&, Callback);
    void put(const Resource& resource, std::shared_ptr<const Response> response);
    void refresh(const Resource& resource, int64_t expires);

private:
    void createDatabase();
    void createSchema();

    const std::string path;
    std::unique_ptr<::mapbox::sqlite::Database> db;
    std::unique_ptr<::mapbox::sqlite::Statement> getStmt;
    std::unique_ptr<::mapbox::sqlite::Statement> putStmt;
    std::unique_ptr<::mapbox::sqlite::Statement> refreshStmt;
    bool schema = false;
};


}

#endif
