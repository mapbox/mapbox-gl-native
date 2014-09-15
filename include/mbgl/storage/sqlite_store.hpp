#ifndef MBGL_STORAGE_SQLITE_STORE
#define MBGL_STORAGE_SQLITE_STORE

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/response.hpp>

#include <uv.h>

#include <string>

typedef struct uv_worker_s uv_worker_t;

namespace mapbox {
namespace sqlite {
class Database;
}
}

namespace mbgl {

class SQLiteStore {
public:
    SQLiteStore(uv_loop_t *loop, const std::string &path);
    ~SQLiteStore();

    typedef void (*GetCallback)(std::unique_ptr<Response> &&entry, void *ptr);

    void get(const std::string &path, GetCallback cb, void *ptr);
    void put(const std::string &path, ResourceType type, const Response &entry);
    void updateExpiration(const std::string &path, int64_t expires);

private:
    void createSchema();
    void closeDatabase();
    static void runGet(uv_work_t *req);
    static void runPut(uv_work_t *req);
    static void deliverResult(uv_work_t *req, int status);

private:
    const unsigned long thread_id;
    std::shared_ptr<mapbox::sqlite::Database> db;
    uv_worker_t *worker = nullptr;
};

}

#endif
