#ifndef MBGL_STORAGE_DEFAULT_SQLITE_CACHE_IMPL
#define MBGL_STORAGE_DEFAULT_SQLITE_CACHE_IMPL

#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

namespace mapbox {
namespace sqlite {
class Database;
class Statement;
}
}

namespace mbgl {

class SQLiteCache::Impl {
public:
    explicit Impl(const std::string &path = ":memory:");
    ~Impl();

    void setMaximumCacheSize(uint64_t size);
    void setMaximumCacheEntrySize(uint64_t size);

    void get(const Resource&, Callback);
    void put(const Resource&, const Response&);
    void refresh(const Resource&, optional<SystemTimePoint> expires);

private:
    void initializeDatabase();

    int cachePageSize();

    uint64_t cacheHardSize();
    uint64_t cacheSoftSize();

    uint64_t softSize = 0;
    bool softSizeDirty = true;

    bool needsPruning();
    void pruneEntries();

    void createDatabase();
    void createSchema();

    int schemaVersion() const;
    void ensureSchemaVersion();

    int pageSize = 0;

    uint64_t maximumCacheSize;
    uint64_t maximumCacheEntrySize;

    const std::string path;
    std::unique_ptr<::mapbox::sqlite::Database> db;
    std::unique_ptr<::mapbox::sqlite::Statement> getStmt;
    std::unique_ptr<::mapbox::sqlite::Statement> putStmt;
    std::unique_ptr<::mapbox::sqlite::Statement> refreshStmt;
    std::unique_ptr<::mapbox::sqlite::Statement> countStmt;
    std::unique_ptr<::mapbox::sqlite::Statement> freeStmt;
    std::unique_ptr<::mapbox::sqlite::Statement> pruneStmt;
    std::unique_ptr<::mapbox::sqlite::Statement> accessedStmt;
    bool schema = false;
};

} // namespace mbgl

#endif
