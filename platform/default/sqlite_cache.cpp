#include "sqlite_cache_impl.hpp"
#include <mbgl/storage/request.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/util/compression.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/platform/log.hpp>

#include "sqlite3.hpp"
#include <sqlite3.h>

namespace mbgl {

using namespace mapbox::sqlite;

SQLiteCache::SQLiteCache(const std::string& path_)
    : thread(std::make_unique<util::Thread<Impl>>(util::ThreadContext{"SQLiteCache", util::ThreadType::Unknown, util::ThreadPriority::Low}, path_)) {
}

SQLiteCache::~SQLiteCache() = default;

SQLiteCache::Impl::Impl(const std::string& path_)
    : path(path_) {
}

SQLiteCache::Impl::~Impl() {
    // Deleting these SQLite objects may result in exceptions, but we're in a destructor, so we
    // can't throw anything.
    try {
        getStmt.reset();
        putStmt.reset();
        refreshStmt.reset();
        db.reset();
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }
}

void SQLiteCache::Impl::createDatabase() {
    db = std::make_unique<Database>(path.c_str(), ReadWrite | Create);
}

void SQLiteCache::Impl::createSchema() {
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
        schema = true;
    } catch (mapbox::sqlite::Exception &ex) {
        if (ex.code == SQLITE_NOTADB) {
            Log::Warning(Event::Database, "Trashing invalid database");
            db.reset();
            try {
                util::deleteFile(path);
            } catch (util::IOException& ioEx) {
                Log::Error(Event::Database, ex.code, ex.what());
            }
            db = std::make_unique<Database>(path.c_str(), ReadWrite | Create);
        } else {
            Log::Error(Event::Database, ex.code, ex.what());
        }

        // Creating the database table + index failed. That means there may already be one, likely
        // with different columsn. Drop it and try to create a new one.
        db->exec("DROP TABLE IF EXISTS `http_cache`");
        db->exec(sql);
    }
}

std::unique_ptr<WorkRequest> SQLiteCache::get(const Resource &resource, Callback callback) {
    // Can be called from any thread, but most likely from the file source thread.
    // Will try to load the URL from the SQLite database and call the callback when done.
    // Note that the callback is probably going to invoked from another thread, so the caller
    // must make sure that it can run in that thread.
    return thread->invokeWithCallback(&Impl::get, callback, resource);
}

void SQLiteCache::Impl::get(const Resource &resource, Callback callback) {
    try {
        // This is called in the SQLite event loop.
        if (!db) {
            createDatabase();
        }

        if (!schema) {
            createSchema();
        }

        if (!getStmt) {
            // Initialize the statement                                  0         1
            getStmt = std::make_unique<Statement>(db->prepare("SELECT `status`, `modified`, "
            //     2         3        4          5                                       1
                "`etag`, `expires`, `data`, `compressed` FROM `http_cache` WHERE `url` = ?"));
        } else {
            getStmt->reset();
        }

        const auto canonicalURL = util::mapbox::canonicalURL(resource.url);
        getStmt->bind(1, canonicalURL.c_str());
        if (getStmt->run()) {
            // There is data.
            auto response = std::make_unique<Response>();
            const auto status = getStmt->get<int>(0);
            if (status > 1) {
                // Status codes > 1 indicate an error
                response->error = std::make_unique<Response::Error>(Response::Error::Reason(status));
            }
            response->modified = getStmt->get<int64_t>(1);
            response->etag = getStmt->get<std::string>(2);
            response->expires = getStmt->get<int64_t>(3);
            response->data = std::make_shared<std::string>(std::move(getStmt->get<std::string>(4)));
            if (getStmt->get<int>(5)) { // == compressed
                response->data = std::make_shared<std::string>(std::move(util::decompress(*response->data)));
            }
            callback(std::move(response));
        } else {
            // There is no data.
            callback(nullptr);
        }
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
        callback(nullptr);
    } catch (std::runtime_error& ex) {
        Log::Error(Event::Database, ex.what());
        callback(nullptr);
    }
}

void SQLiteCache::put(const Resource &resource, std::shared_ptr<const Response> response, Hint hint) {
    // Can be called from any thread, but most likely from the file source thread. We are either
    // storing a new response or updating the currently stored response, potentially setting a new
    // expiry date.
    if (hint == Hint::Full) {
        thread->invoke(&Impl::put, resource, response);
    } else if (hint == Hint::Refresh) {
        thread->invoke(&Impl::refresh, resource, response->expires);
    }
}

void SQLiteCache::Impl::put(const Resource& resource, std::shared_ptr<const Response> response) {
    try {
        if (!db) {
            createDatabase();
        }

        if (!schema) {
            createSchema();
        }

        if (!putStmt) {
            putStmt = std::make_unique<Statement>(db->prepare("REPLACE INTO `http_cache` ("
            //     1       2       3         4         5         6        7          8
                "`url`, `status`, `kind`, `modified`, `etag`, `expires`, `data`, `compressed`"
                ") VALUES(?, ?, ?, ?, ?, ?, ?, ?)"));
        } else {
            putStmt->reset();
        }

        const auto canonicalURL = util::mapbox::canonicalURL(resource.url);
        putStmt->bind(1 /* url */, canonicalURL.c_str());
        if (response->error) {
            putStmt->bind(2 /* status */, int(response->error->reason));
        } else {
            putStmt->bind(2 /* status */, 1 /* success */);
        }
        putStmt->bind(3 /* kind */, int(resource.kind));
        putStmt->bind(4 /* modified */, response->modified);
        putStmt->bind(5 /* etag */, response->etag.c_str());
        putStmt->bind(6 /* expires */, response->expires);

        std::string data;
        if (resource.kind != Resource::SpriteImage && response->data) {
            // Do not compress images, since they are typically compressed already.
            data = util::compress(*response->data);
        }

        if (!data.empty() && data.size() < response->data->size()) {
            // Store the compressed data when it is smaller than the original
            // uncompressed data.
            putStmt->bind(7 /* data */, data, false); // do not retain the string internally.
            putStmt->bind(8 /* compressed */, true);
        } else if (response->data) {
            putStmt->bind(7 /* data */, *response->data, false); // do not retain the string internally.
            putStmt->bind(8 /* compressed */, false);
        } else {
            putStmt->bind(7 /* data */, "", false);
            putStmt->bind(8 /* compressed */, false);
        }

        putStmt->run();
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    } catch (std::runtime_error& ex) {
        Log::Error(Event::Database, ex.what());
    }
}

void SQLiteCache::Impl::refresh(const Resource& resource, int64_t expires) {
    try {
        if (!db) {
            createDatabase();
        }

        if (!schema) {
            createSchema();
        }

        if (!refreshStmt) {
            refreshStmt = std::make_unique<Statement>( //        1               2
                db->prepare("UPDATE `http_cache` SET `expires` = ? WHERE `url` = ?"));
        } else {
            refreshStmt->reset();
        }

        const auto canonicalURL = util::mapbox::canonicalURL(resource.url);
        refreshStmt->bind(1, int64_t(expires));
        refreshStmt->bind(2, canonicalURL.c_str());
        refreshStmt->run();
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }
}

std::shared_ptr<SQLiteCache> SharedSQLiteCache::get(const std::string &path) {
    std::shared_ptr<SQLiteCache> temp = masterPtr.lock();
    if (!temp) {
        temp.reset(new SQLiteCache(path));
        masterPtr = temp;
    }

    return temp;
}

std::weak_ptr<SQLiteCache> SharedSQLiteCache::masterPtr;

}
