#include <mbgl/storage/sqlite_store.hpp>
#include <mbgl/util/compression.hpp>
#include <mbgl/util/sqlite3.hpp>

#include <mbgl/util/uv-worker.h>

#include <cassert>

using namespace mapbox::sqlite;

std::string removeAccessTokenFromURL(const std::string &url) {
    const size_t token_start = url.find("access_token=");
    // Ensure that token exists, isn't at the front and is preceded by either & or ?.
    if (token_start == std::string::npos || token_start == 0 || !(url[token_start - 1] == '&' || url[token_start - 1] == '?')) {
        return url;
    }

    const size_t token_end = url.find_first_of('&', token_start);
    if (token_end == std::string::npos) {
        // The token is the last query argument. We slice away the "&access_token=..." part
        return url.substr(0, token_start - 1);
    } else {
        // We slice away the "access_token=...&" part.
        return url.substr(0, token_start) + url.substr(token_end + 1);
    }
}

namespace mbgl {

SQLiteStore::SQLiteStore(uv_loop_t *loop, const std::string &path)
    : thread_id(uv_thread_self()),
      db(std::make_shared<Database>(path.c_str(), ReadWrite | Create)) {
    createSchema();
    worker = new uv_worker_t;
    uv_worker_init(worker, loop, 1, "SQLite");
}

SQLiteStore::~SQLiteStore() {
    // Nothing to do. This function needs to be here because we're forward-declaring
    // Database, so we need the actual definition here to be able to properly destruct it.
    if (worker) {
        uv_worker_close(worker);
        delete worker;
    }
}

void SQLiteStore::createSchema() {
    if (!db || !*db) {
        return;
    }

    db->exec("CREATE TABLE IF NOT EXISTS `http_cache` ("
             "    `url` TEXT PRIMARY KEY NOT NULL,"
             "    `code` INTEGER NOT NULL,"
             "    `type` INTEGER NOT NULL,"
             "    `modified` INTEGER,"
             "    `expires` INTEGER,"
             "    `data` BLOB,"
             "    `compressed` INTEGER NOT NULL DEFAULT 0"
             ");"
             "CREATE INDEX IF NOT EXISTS `http_cache_type_idx` ON `http_cache` (`type`);");
}

struct GetBaton {
    util::ptr<Database> db;
    std::string path;
    ResourceType type;
    void *ptr = nullptr;
    SQLiteStore::GetCallback callback = nullptr;
    std::unique_ptr<Response> response;
};

void SQLiteStore::get(const std::string &path, GetCallback callback, void *ptr) {
    assert(uv_thread_self() == thread_id);
    if (!db || !*db) {
        if (callback) {
            callback(nullptr, ptr);
        }
        return;
    }

    GetBaton *get_baton = new GetBaton;
    get_baton->db = db;
    get_baton->path = path;
    get_baton->ptr = ptr;
    get_baton->callback = callback;

    uv_worker_send(worker, get_baton, [](void *data) {
        GetBaton *baton = (GetBaton *)data;
        const std::string url = removeAccessTokenFromURL(baton->path);
        //                                                    0       1         2
        Statement stmt = baton->db->prepare("SELECT `code`, `type`, `modified`, "
        //       3        4          5
            "`expires`, `data`, `compressed` FROM `http_cache` WHERE `url` = ?");

        stmt.bind(1, url.c_str());
        if (stmt.run()) {
            // There is data.
            baton->response = std::unique_ptr<Response>(new Response);

            baton->response->code = stmt.get<int>(0);
            baton->type = ResourceType(stmt.get<int>(1));
            baton->response->modified = stmt.get<int64_t>(2);
            baton->response->expires = stmt.get<int64_t>(3);
            baton->response->data = stmt.get<std::string>(4);
            if (stmt.get<int>(5)) { // == compressed
                baton->response->data = util::decompress(baton->response->data);
            }
        } else {
            // There is no data.
            // This is a noop.
        }
    }, [](void *data) {
        std::unique_ptr<GetBaton> baton { (GetBaton *)data };
        if (baton->callback) {
            baton->callback(std::move(baton->response), baton->ptr);
        }
    });
}


struct PutBaton {
    util::ptr<Database> db;
    std::string path;
    ResourceType type;
    Response response;
};

void SQLiteStore::put(const std::string &path, ResourceType type, const Response &response) {
    assert(uv_thread_self() == thread_id);
    if (!db) return;

    PutBaton *put_baton = new PutBaton;
    put_baton->db = db;
    put_baton->path = path;
    put_baton->type = type;
    put_baton->response = response;

    uv_worker_send(worker, put_baton, [](void *data) {
        PutBaton *baton = (PutBaton *)data;
        const std::string url = removeAccessTokenFromURL(baton->path);
        Statement stmt = baton->db->prepare("REPLACE INTO `http_cache` ("
        //     1      2       3         4           5        6          7
            "`url`, `code`, `type`, `modified`, `expires`, `data`, `compressed`"
            ") VALUES(?, ?, ?, ?, ?, ?, ?)");
        stmt.bind(1, url.c_str());
        stmt.bind(2, int(baton->response.code));
        stmt.bind(3, int(baton->type));
        stmt.bind(4, baton->response.modified);
        stmt.bind(5, baton->response.expires);

        if (baton->type == ResourceType::Image) {
            stmt.bind(6, baton->response.data, false); // do not retain the string internally.
            stmt.bind(7, false);
        } else {
            stmt.bind(6, util::compress(baton->response.data), true); // retain the string internally.
            stmt.bind(7, true);
        }

        stmt.run();
    }, [](void *data) {
        delete (PutBaton *)data;
    });
}

struct ExpirationBaton {
    util::ptr<Database> db;
    std::string path;
    int64_t expires;
};

void SQLiteStore::updateExpiration(const std::string &path, int64_t expires) {
    assert(uv_thread_self() == thread_id);
    if (!db || !*db) return;

    ExpirationBaton *expiration_baton = new ExpirationBaton;
    expiration_baton->db = db;
    expiration_baton->path = path;
    expiration_baton->expires = expires;

    uv_worker_send(worker, expiration_baton, [](void *data) {
        ExpirationBaton *baton = (ExpirationBaton *)data;
        const std::string url = removeAccessTokenFromURL(baton->path);
        Statement stmt = //                                 1               2
            baton->db->prepare("UPDATE `http_cache` SET `expires` = ? WHERE `url` = ?");
        stmt.bind<int64_t>(1, baton->expires);
        stmt.bind(2, url.c_str());
        stmt.run();
    }, [](void *data) {
        delete (ExpirationBaton *)data;
    });
}

}
