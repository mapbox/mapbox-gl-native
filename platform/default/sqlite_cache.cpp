#include "sqlite_cache_impl.hpp"
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/util/compression.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/platform/log.hpp>

#include "sqlite3.hpp"
#include <sqlite3.h>

#include <unordered_map>
#include <mutex>

namespace {

// The cache won't accept entries larger than this arbitrary size
// and will silently discard request for adding them to the cache.
// Large entries can cause the database to grow in disk size and
// never shrink again.
const uint64_t kMaximumCacheEntrySize = 5 * 1024 * 1024; // 5 MB

// Number of records we delete when we are close to the maximum
// database size, if set. The current criteria is to prune
// the least used entries based on `accessed` time.
const int kPrunedEntriesLimit = 100;

} // namespace

namespace mbgl {

using namespace mapbox::sqlite;

SQLiteCache::SQLiteCache(const std::string& path_)
    : thread(std::make_unique<util::Thread<Impl>>(util::ThreadContext{"SQLiteCache", util::ThreadType::Unknown, util::ThreadPriority::Low}, path_)) {
}

SQLiteCache::~SQLiteCache() = default;

SQLiteCache::Impl::Impl(const std::string& path_)
    : maximumCacheSize(0), // Unlimited
      maximumCacheEntrySize(kMaximumCacheEntrySize),
      path(path_) {
}

SQLiteCache::Impl::~Impl() {
    // Deleting these SQLite objects may result in exceptions, but we're in a destructor, so we
    // can't throw anything.
    try {
        getStmt.reset();
        putStmt.reset();
        refreshStmt.reset();
        countStmt.reset();
        freeStmt.reset();
        pruneStmt.reset();
        accessedStmt.reset();
        db.reset();
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }
}

void SQLiteCache::Impl::createDatabase() {
    db = std::make_unique<Database>(path.c_str(), ReadWrite | Create);
}

int SQLiteCache::Impl::schemaVersion() const {
    // WARNING: Bump the version when changing the cache
    // scheme to force the table to be recreated.
    return 1;
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
        "    `accessed` INTEGER," // Timestamp when the database record was last accessed.
        "    `data` BLOB,"
        "    `compressed` INTEGER NOT NULL DEFAULT 0" // Whether the data is compressed.
        ");"
        "CREATE INDEX IF NOT EXISTS `http_cache_kind_idx` ON `http_cache` (`kind`);"
        "CREATE INDEX IF NOT EXISTS `http_cache_accessed_idx` ON `http_cache` (`accessed`);";

    ensureSchemaVersion();

    try {
        db->exec(sql);
        db->exec("PRAGMA user_version = " + util::toString(schemaVersion()));
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
        // with different columns. Drop it and try to create a new one.
        db->exec("DROP TABLE IF EXISTS `http_cache`");
        db->exec(sql);
        db->exec("PRAGMA user_version = " + util::toString(schemaVersion()));
    }
}

void SQLiteCache::Impl::ensureSchemaVersion() {
    try {
       Statement userVersionStmt(db->prepare("PRAGMA user_version"));
       if (userVersionStmt.run() && userVersionStmt.get<int>(0) == schemaVersion()) {
           return;
       }
    } catch (mapbox::sqlite::Exception& ex) {
        if (ex.code == SQLITE_NOTADB) {
            return;
        }

        Log::Error(Event::Database, ex.code, ex.what());
    }

    // Version mismatch, drop the table so it will
    // get recreated by `createSchema()`.
    try {
        db->exec("DROP TABLE IF EXISTS `http_cache`");
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }
}

void SQLiteCache::setMaximumCacheSize(uint64_t size) {
    thread->invoke(&Impl::setMaximumCacheSize, size);
}

void SQLiteCache::Impl::setMaximumCacheSize(uint64_t size) {
    maximumCacheSize = size;

    // Unlimited.
    if (size == 0) {
        return;
    }

    uint64_t lastSoftSize = cacheSoftSize();

    // Keep pruning until we fit in the new
    // size limit.
    while (lastSoftSize > maximumCacheSize) {
        pruneEntries();

        if (lastSoftSize != cacheSoftSize()) {
            lastSoftSize = cacheSoftSize();
        } else {
            break;
        }
    }

    if (cacheHardSize() > size) {
        Log::Warning(mbgl::Event::Database,
            "Current cache hard size is bigger than the defined "
            "maximum size. Database won't get truncated.");
    }
}

void SQLiteCache::setMaximumCacheEntrySize(uint64_t size) {
    thread->invoke(&Impl::setMaximumCacheEntrySize, size);
}

void SQLiteCache::Impl::setMaximumCacheEntrySize(uint64_t size) {
    maximumCacheEntrySize = size;
}

void SQLiteCache::Impl::initializeDatabase() {
    if (!db) {
        createDatabase();
    }

    if (!schema) {
        createSchema();
    }
}

int SQLiteCache::Impl::cachePageSize() {
    try {
        if (!pageSize) {
            Statement pageSizeStmt(db->prepare("PRAGMA page_size"));
            if (pageSizeStmt.run()) {
                pageSize = pageSizeStmt.get<int>(0);
            }
        }
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }

    return pageSize;
}

uint64_t SQLiteCache::Impl::cacheHardSize() {
    try {
        initializeDatabase();

        if (!countStmt) {
            countStmt = std::make_unique<Statement>(db->prepare("PRAGMA page_count"));
        } else {
            countStmt->reset();
        }

        if (countStmt->run()) {
            return cachePageSize() * countStmt->get<int>(0);
        }
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }

    return 0;
}

uint64_t SQLiteCache::Impl::cacheSoftSize() {
    if (!softSizeDirty) {
        return softSize;
    }

    try {
        initializeDatabase();

        if (!freeStmt) {
            freeStmt = std::make_unique<Statement>(db->prepare("PRAGMA freelist_count"));
        } else {
            freeStmt->reset();
        }

        uint64_t hardSize = cacheHardSize();
        if (!hardSize) {
            return 0;
        }

        if (freeStmt->run()) {
            return hardSize - cachePageSize() * freeStmt->get<int>(0);
        }

        softSizeDirty = false;
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }

    return 0;
}

bool SQLiteCache::Impl::needsPruning() {
    // SQLite database never shrinks in size unless we call VACCUM. We here
    // are monitoring the soft limit (i.e. number of free pages in the file)
    // and as it approaches to the hard limit (i.e. the actual file size) we
    // delete an arbitrary number of old cache entries.
    //
    // The free pages approach saves us from calling VACCUM or keeping a
    // running total, which can be costly. We need a buffer because pages can
    // get fragmented on the database.
    if (cacheSoftSize() + maximumCacheEntrySize * 2 < maximumCacheSize) {
        return false;
    } else {
        return true;
    }
}

void SQLiteCache::Impl::pruneEntries() {
    if (!maximumCacheSize) {
        return;
    }

    if (!needsPruning()) {
        return;
    }

    try {
        if (!pruneStmt) {
            pruneStmt = std::make_unique<Statement>(db->prepare(
                "DELETE FROM `http_cache` WHERE `rowid` IN (SELECT `rowid` FROM "
                //                                          1
                "`http_cache` ORDER BY `accessed` ASC LIMIT ?)"));
        } else {
            pruneStmt->reset();
        }

        pruneStmt->bind(1, kPrunedEntriesLimit);

        pruneStmt->run();
        softSizeDirty = true;
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
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
        initializeDatabase();

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
            response->modified = Seconds(getStmt->get<int64_t>(1));
            response->etag = getStmt->get<std::string>(2);
            response->expires = Seconds(getStmt->get<int64_t>(3));
            response->data = std::make_shared<std::string>(getStmt->get<std::string>(4));
            if (getStmt->get<int>(5)) { // == compressed
                response->data = std::make_shared<std::string>(util::decompress(*response->data));
            }
            callback(std::move(response));
        } else {
            // There is no data.
            callback(nullptr);
        }

        // We do an extra query for refreshing the last time
        // the record was accessed that can be costly and is only
        // worth doing if we are monitoring the database size.
        if (maximumCacheSize) {
            if (!accessedStmt) {
                accessedStmt = std::make_unique<Statement>(
                    //                                                1               2
                    db->prepare("UPDATE `http_cache` SET `accessed` = ? WHERE `url` = ?"));
            } else {
                accessedStmt->reset();
            }

            accessedStmt->bind(1, int64_t(toSeconds(SystemClock::now()).count()));
            accessedStmt->bind(2, canonicalURL.c_str());
            accessedStmt->run();
        }
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
        callback(nullptr);
    } catch (std::runtime_error& ex) {
        Log::Error(Event::Database, "%s", ex.what());
        callback(nullptr);
    }
}

void SQLiteCache::put(const Resource& resource, const Response& response) {
    // Except for 404s, don't store errors in the cache.
    if (response.error && response.error->reason != Response::Error::Reason::NotFound) {
        return;
    }

    if (response.notModified) {
        thread->invoke(&Impl::refresh, resource, response.expires);
    } else {
        thread->invoke(&Impl::put, resource, response);
    }
}

void SQLiteCache::Impl::put(const Resource& resource, const Response& response) {
    try {
        initializeDatabase();
        pruneEntries();

        if (response.data) {
            auto entrySize = response.data->size();

            if (entrySize > maximumCacheEntrySize) {
                Log::Warning(Event::Database, "Entry too big for caching.");
                return;
            }

            if (maximumCacheSize && entrySize + cacheSoftSize() > maximumCacheSize) {
                Log::Warning(Event::Database, "Unable to make space for new entries.");
                return;
            }
        }

        if (!putStmt) {
            putStmt = std::make_unique<Statement>(db->prepare("REPLACE INTO `http_cache` ("
                // 1        2       3         4         5         6          7         8          9
                "`url`, `status`, `kind`, `modified`, `etag`, `expires`, `accessed`, `data`, `compressed`"
                ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)"));
        } else {
            putStmt->reset();
        }

        const auto canonicalURL = util::mapbox::canonicalURL(resource.url);
        putStmt->bind(1 /* url */, canonicalURL.c_str());
        if (response.error) {
            putStmt->bind(2 /* status */, int(response.error->reason));
        } else {
            putStmt->bind(2 /* status */, 1 /* success */);
        }
        putStmt->bind(3 /* kind */, int(resource.kind));
        putStmt->bind(4 /* modified */, int64_t(response.modified.count()));
        putStmt->bind(5 /* etag */, response.etag.c_str());
        putStmt->bind(6 /* expires */, int64_t(response.expires.count()));
        putStmt->bind(7 /* accessed */, int64_t(toSeconds(SystemClock::now()).count()));

        std::string data;
        if (resource.kind != Resource::SpriteImage && response.data) {
            // Do not compress images, since they are typically compressed already.
            data = util::compress(*response.data);
        }

        if (!data.empty() && data.size() < response.data->size()) {
            // Store the compressed data when it is smaller than the original
            // uncompressed data.
            putStmt->bind(8 /* data */, data, false); // do not retain the string internally.
            putStmt->bind(9 /* compressed */, true);
        } else if (response.data) {
            putStmt->bind(8 /* data */, *response.data, false); // do not retain the string internally.
            putStmt->bind(9 /* compressed */, false);
        } else {
            putStmt->bind(8 /* data */, "", false);
            putStmt->bind(9 /* compressed */, false);
        }

        putStmt->run();
        softSizeDirty = true;
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    } catch (std::runtime_error& ex) {
        Log::Error(Event::Database, "%s", ex.what());
    }
}

void SQLiteCache::Impl::refresh(const Resource& resource, Seconds expires) {
    try {
        initializeDatabase();

        if (!refreshStmt) {
            refreshStmt = std::make_unique<Statement>(
                db->prepare("UPDATE `http_cache` SET "
                    //            1              2               3
                    "`accessed` = ?, `expires` = ? WHERE `url` = ?"));
        } else {
            refreshStmt->reset();
        }

        const auto canonicalURL = util::mapbox::canonicalURL(resource.url);
        refreshStmt->bind(1, int64_t(toSeconds(SystemClock::now()).count()));
        refreshStmt->bind(2, int64_t(expires.count()));
        refreshStmt->bind(3, canonicalURL.c_str());
        refreshStmt->run();
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }
}

namespace {

static std::mutex sharedMutex;
static std::unordered_map<std::string, std::weak_ptr<SQLiteCache>> shared;

} // namespace

std::shared_ptr<SQLiteCache> SQLiteCache::getShared(const std::string &path) {
    std::lock_guard<std::mutex> lock(sharedMutex);

    std::shared_ptr<SQLiteCache> cache;

    auto it = shared.find(path);
    if (it != shared.end()) {
        cache = it->second.lock();
        if (!cache) {
            cache = std::make_shared<SQLiteCache>(path);
            it->second = cache;
        }
    } else {
        cache = std::make_shared<SQLiteCache>(path);
        shared.emplace(path, cache);
    }

    return cache;
}

} // namespace mbgl
