#include <mbgl/storage/default/sqlite_cache.hpp>
#include <mbgl/storage/default/request.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/util/util.hpp>
#include <mbgl/util/async_queue.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/platform/log.hpp>

#include "sqlite3.hpp"
#include "compression.hpp"

#include <uv.h>

#include <cassert>

namespace mbgl {

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

std::string convertMapboxDomainsToProtocol(const std::string &url) {
    const size_t protocol_separator = url.find("://");
    if (protocol_separator == std::string::npos) {
        return url;
    }

    const std::string protocol = url.substr(0, protocol_separator);
    if (!(protocol == "http" || protocol == "https")) {
        return url;
    }

    const size_t domain_begin = protocol_separator + 3;
    const size_t path_separator = url.find("/", domain_begin);
    if (path_separator == std::string::npos) {
        return url;
    }

    const std::string domain = url.substr(domain_begin, path_separator - domain_begin);
    if (domain.find(".tiles.mapbox.com") != std::string::npos) {
        return "mapbox://" + url.substr(path_separator + 1);
    } else {
        return url;
    }
}

std::string unifyMapboxURLs(const std::string &url) {
    return removeAccessTokenFromURL(convertMapboxDomainsToProtocol(url));
}


using namespace mapbox::sqlite;

struct SQLiteCache::GetAction {
    const Resource resource;
    const std::function<void(std::unique_ptr<Response>)> callback;
};

struct SQLiteCache::PutAction {
    const Resource resource;
    const std::shared_ptr<const Response> response;
};

struct SQLiteCache::RefreshAction {
    const Resource resource;
    const int64_t expires;
};

struct SQLiteCache::StopAction {
};

struct SQLiteCache::ActionDispatcher {
    SQLiteCache &cache;
    template <typename T> void operator()(T &t) { cache.process(t); }
};

SQLiteCache::SQLiteCache(const std::string &path_)
    : path(path_),
      loop(uv_loop_new()),
      queue(new Queue(loop, [this](Action &action) {
          mapbox::util::apply_visitor(ActionDispatcher{ *this }, action);
      })),
      thread([this]() {
#ifdef __APPLE__
          pthread_setname_np("SQLite Cache");
#endif
          uv_run(loop, UV_RUN_DEFAULT);
      })
{
}

SQLiteCache::~SQLiteCache() {
    if (thread.joinable()) {
        if (queue) {
            queue->send(StopAction{ });
        }
        thread.join();
        uv_loop_delete(loop);
    }
}


void SQLiteCache::get(const Resource &resource, std::function<void(std::unique_ptr<Response>)> callback) {
    // Can be called from any thread, but most likely from the file source thread.
    // Will try to load the URL from the SQLite database and call the callback when done.
    // Note that the callback is probably going to invoked from another thread, so the caller
    // must make sure that it can run in that thread.
    assert(queue);
    queue->send(GetAction{ resource, callback });
}

void SQLiteCache::put(const Resource &resource, std::shared_ptr<const Response> response, Hint hint) {
    // Can be called from any thread, but most likely from the file source thread. We are either
    // storing a new response or updating the currently stored response, potentially setting a new
    // expiry date.
    assert(queue);
    assert(response);

    if (hint == Hint::Full) {
        queue->send(PutAction{ resource, response });
    } else if (hint == Hint::Refresh) {
        queue->send(RefreshAction{ resource, response->expires });
    }
}

void SQLiteCache::createDatabase() {
    db = util::make_unique<Database>(path.c_str(), ReadWrite | Create);

    constexpr const char *const sql = ""
        "CREATE TABLE IF NOT EXISTS `http_cache` ("
        "    `url` TEXT PRIMARY KEY NOT NULL,"
        "    `status` INTEGER NOT NULL," // The response status (Successful or Error).
        "    `kind` INTEGER NOT NULL," // The kind of file.
        "    `modified` INTEGER," // Timestamp when the file was last modified.
        "    `etag` TEXT,"
        "    `expires` INTEGER," // Timestamp when the server says the file expires.
        "    `data` BLOB,"
        "    `compressed` INTEGER NOT NULL DEFAULT 0" // Whether the data is compressed.
        ");"
        "CREATE INDEX IF NOT EXISTS `http_cache_kind_idx` ON `http_cache` (`kind`);";

    try {
        db->exec(sql);
    } catch(mapbox::sqlite::Exception &) {
        // Creating the database table + index failed. That means there may already be one, likely
        // with different columsn. Drop it and try to create a new one.
        try {
            db->exec("DROP TABLE IF EXISTS `http_cache`");
            db->exec(sql);
        } catch (mapbox::sqlite::Exception &ex) {
            Log::Error(Event::Database, "Failed to create database: %s", ex.what());
            db.release();
        }
    }
}

void SQLiteCache::process(GetAction &action) {
    // This is called in the SQLite event loop.
    if (!db) {
        createDatabase();
    }

    if (!getStmt) {
        // Initialize the statement                                   0         1
        getStmt = util::make_unique<Statement>(db->prepare("SELECT `status`, `modified`, "
        //     2         3        4          5                                       1
            "`etag`, `expires`, `data`, `compressed` FROM `http_cache` WHERE `url` = ?"));
    } else {
        getStmt->reset();
    }

    const std::string unifiedURL = unifyMapboxURLs(action.resource.url);
    getStmt->bind(1, unifiedURL.c_str());
    if (getStmt->run()) {
        // There is data.
        auto response = util::make_unique<Response>();
        response->status = Response::Status(getStmt->get<int>(0));
        response->modified = getStmt->get<int64_t>(1);
        response->etag = getStmt->get<std::string>(2);
        response->expires = getStmt->get<int64_t>(3);
        response->data = getStmt->get<std::string>(4);
        if (getStmt->get<int>(5)) { // == compressed
            response->data = util::decompress(response->data);
        }
        action.callback(std::move(response));
    } else {
        // There is no data.
        action.callback(nullptr);
    }
}

void SQLiteCache::process(PutAction &action) {
    if (!db) {
        createDatabase();
    }

    if (!putStmt) {
        putStmt = util::make_unique<Statement>(db->prepare("REPLACE INTO `http_cache` ("
        //     1       2       3         4         5         6        7          8
            "`url`, `status`, `kind`, `modified`, `etag`, `expires`, `data`, `compressed`"
            ") VALUES(?, ?, ?, ?, ?, ?, ?, ?)"));
    } else {
        putStmt->reset();
    }

    const std::string unifiedURL = unifyMapboxURLs(action.resource.url);
    putStmt->bind(1 /* url */, unifiedURL.c_str());
    putStmt->bind(2 /* status */, int(action.response->status));
    putStmt->bind(3 /* kind */, int(action.resource.kind));
    putStmt->bind(4 /* modified */, action.response->modified);
    putStmt->bind(5 /* etag */, action.response->etag.c_str());
    putStmt->bind(6 /* expires */, action.response->expires);

    std::string data;
    if (action.resource.kind != Resource::Image) {
        // Do not compress images, since they are typically compressed already.
        data = util::compress(action.response->data);
    }

    if (!data.empty() && data.size() < action.response->data.size()) {
        // Store the compressed data when it is smaller than the original
        // uncompressed data.
        putStmt->bind(7 /* data */, data, false); // do not retain the string internally.
        putStmt->bind(8 /* compressed */, true);
    } else {
        putStmt->bind(7 /* data */, action.response->data, false); // do not retain the string internally.
        putStmt->bind(8 /* compressed */, false);
    }

    putStmt->run();
}

void SQLiteCache::process(RefreshAction &action) {
    if (!db) {
        createDatabase();
    }

    if (!refreshStmt) {
        refreshStmt = util::make_unique<Statement>( //       1               2
            db->prepare("UPDATE `http_cache` SET `expires` = ? WHERE `url` = ?"));
    } else {
        refreshStmt->reset();
    }

    const std::string unifiedURL = unifyMapboxURLs(action.resource.url);
    refreshStmt->bind(1, int64_t(action.expires));
    refreshStmt->bind(2, unifiedURL.c_str());
    refreshStmt->run();
}

void SQLiteCache::process(StopAction &) {
    assert(queue);
    queue->stop();
    queue = nullptr;
}

}
