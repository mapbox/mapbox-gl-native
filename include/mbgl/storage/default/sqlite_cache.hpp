#ifndef MBGL_STORAGE_DEFAULT_SQLITE_CACHE
#define MBGL_STORAGE_DEFAULT_SQLITE_CACHE

#include <mbgl/storage/file_cache.hpp>
#include <mbgl/util/run_loop.hpp>

#include <string>

namespace mapbox { namespace sqlite { class Database; class Statement; } }

namespace mbgl {

class SQLiteCache : public FileCache, protected util::RunLoop {
    friend class util::Thread<SQLiteCache>;

private:
    SQLiteCache(const std::string &path = ":memory:");
    ~SQLiteCache();

public:
    // FileCache API
    void get(const Resource &resource, Callback callback) override;
    void put(const Resource &resource, std::shared_ptr<const Response> response, Hint hint) override;

private:
    void createDatabase();
    void createSchema();

    void processGet(const Resource& resource, Callback callback);
    void processPut(const Resource& resource, std::shared_ptr<const Response> response);
    void processRefresh(const Resource& resource, int64_t expires);

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
