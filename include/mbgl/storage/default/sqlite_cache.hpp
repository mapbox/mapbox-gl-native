#ifndef MBGL_STORAGE_DEFAULT_SQLITE_CACHE
#define MBGL_STORAGE_DEFAULT_SQLITE_CACHE

#include <mbgl/storage/file_cache.hpp>

#include <string>
#include <thread>

typedef struct uv_loop_s uv_loop_t;

namespace mapbox { namespace util { template<typename... Types> class variant; } }
namespace mapbox { namespace sqlite { class Database; class Statement; } }

namespace mbgl {

namespace util { template <typename T> class AsyncQueue; }

class SQLiteCache : public FileCache {
    struct GetAction;
    struct PutAction;
    struct RefreshAction;
    struct StopAction;
    using Action = mapbox::util::variant<GetAction, PutAction, RefreshAction, StopAction>;
    using Queue = util::AsyncQueue<Action>;

public:
    SQLiteCache(const std::string &path = ":memory:");
    ~SQLiteCache();

    void get(const Resource &resource, std::function<void(std::unique_ptr<Response>)> callback);
    void put(const Resource &resource, std::shared_ptr<const Response> response, Hint hint);

private:
    struct ActionDispatcher;
    void process(GetAction &action);
    void process(PutAction &action);
    void process(RefreshAction &action);
    void process(StopAction &action);

    void createDatabase();
    void createSchema();

    const std::string path;
    uv_loop_t *loop = nullptr;
    Queue *queue = nullptr;
    std::thread thread;
    std::unique_ptr<::mapbox::sqlite::Database> db;
    std::unique_ptr<::mapbox::sqlite::Statement> getStmt, putStmt, refreshStmt;
    bool schema = false;
};

}

#endif
