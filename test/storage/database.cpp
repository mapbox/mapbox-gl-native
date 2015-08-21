#include "storage.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include "sqlite_cache_impl.hpp"
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/io.hpp>

#include <sqlite3.h>

TEST_F(Storage, DatabaseDoesNotExist) {
    using namespace mbgl;

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    SQLiteCache::Impl cache("test/fixtures/404/cache.db");

    cache.get({ Resource::Unknown, "mapbox://test" }, [] (std::unique_ptr<Response> res) {
        EXPECT_EQ(nullptr, res.get());
    });

    auto observer = Log::removeObserver();
    EXPECT_EQ(1ul, dynamic_cast<FixtureLogObserver*>(observer.get())->count({ EventSeverity::Error, Event::Database, 14, "unable to open database file" }));
}

void createDir(const char* name) {
    const int ret = mkdir(name, 0755);
    if (ret == -1) {
        ASSERT_EQ(EEXIST, errno);
    } else {
        ASSERT_EQ(0, ret);
    }
}

void deleteFile(const char* name) {
    const int ret = unlink(name);
    if (ret == -1) {
        ASSERT_EQ(ENOENT, errno);
    } else {
        ASSERT_EQ(0, ret);
    }
}


void writeFile(const char* name, const std::string& data) {
    mbgl::util::write_file(name, data);
}


TEST_F(Storage, DatabaseCreate) {
    using namespace mbgl;

    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/cache.db");

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    SQLiteCache::Impl cache("test/fixtures/database/cache.db");

    cache.get({ Resource::Unknown, "mapbox://test" }, [] (std::unique_ptr<Response> res) {
        EXPECT_EQ(nullptr, res.get());
    });

    Log::removeObserver();
}

class FileLock {
public:
    FileLock(const std::string& path) {
        const int err = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr);
        if (err != SQLITE_OK) {
            throw std::runtime_error("Could not open db");
        }
        lock();
    }

    void lock() {
        assert(!locked);
        const int err = sqlite3_exec(db, "begin exclusive transaction", nullptr, nullptr, nullptr);
        if (err != SQLITE_OK) {
            throw std::runtime_error("Could not lock db");
        }
        locked = true;
    }

    void unlock() {
        assert(locked);
        const int err = sqlite3_exec(db, "commit", nullptr, nullptr, nullptr);
        if (err != SQLITE_OK) {
            throw std::runtime_error("Could not unlock db");
        }
        locked = false;
    }

    ~FileLock() {
        if (locked) {
            unlock();
        }
    }

private:
    sqlite3* db;
    bool locked = false;
};

TEST_F(Storage, DatabaseLockedRead) {
    using namespace mbgl;

    // Create a locked file.
    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/locked.db");
    FileLock guard("test/fixtures/database/locked.db");

    SQLiteCache::Impl cache("test/fixtures/database/locked.db");

    {
        // First request should fail.
        Log::setObserver(std::make_unique<FixtureLogObserver>());

        cache.get({ Resource::Unknown, "mapbox://test" }, [] (std::unique_ptr<Response> res) {
            EXPECT_EQ(nullptr, res.get());
        });

        // Make sure that we got a few "database locked" errors
        auto observer = Log::removeObserver();
        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
        EXPECT_EQ(2ul, flo->count({ EventSeverity::Error, Event::Database, 5, "database is locked" }));
    }

    // Then, unlock the file and try again.
    guard.unlock();

    {
        // First, try getting a file (the cache value should not exist).
        Log::setObserver(std::make_unique<FixtureLogObserver>());

        cache.get({ Resource::Unknown, "mapbox://test" }, [] (std::unique_ptr<Response> res) {
            EXPECT_EQ(nullptr, res.get());
        });

        // Make sure that we got a no errors
        Log::removeObserver();
    }
}



TEST_F(Storage, DatabaseLockedWrite) {
    using namespace mbgl;

    // Create a locked file.
    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/locked.db");
    FileLock guard("test/fixtures/database/locked.db");

    SQLiteCache::Impl cache("test/fixtures/database/locked.db");

    {
        // Adds a file (which should fail).
        Log::setObserver(std::make_unique<FixtureLogObserver>());

        auto response = std::make_shared<Response>();
        cache.put({ Resource::Unknown, "mapbox://test" }, response);
        cache.get({ Resource::Unknown, "mapbox://test" }, [] (std::unique_ptr<Response> res) {
            EXPECT_EQ(nullptr, res.get());
        });

        auto observer = Log::removeObserver();
        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
        EXPECT_EQ(4ul, flo->count({ EventSeverity::Error, Event::Database, 5, "database is locked" }));
    }

    // Then, unlock the file and try again.
    guard.unlock();

    {
        // Then, set a file and obtain it again.
        Log::setObserver(std::make_unique<FixtureLogObserver>());

        auto response = std::make_shared<Response>();
        response->data = "Demo";
        cache.put({ Resource::Unknown, "mapbox://test" }, response);
        cache.get({ Resource::Unknown, "mapbox://test" }, [] (std::unique_ptr<Response> res) {
            EXPECT_NE(nullptr, res.get());
            EXPECT_EQ("Demo", res->data);
        });

        // Make sure that we got a no errors
        Log::removeObserver();
    }
}




TEST_F(Storage, DatabaseLockedRefresh) {
    using namespace mbgl;

    // Create a locked file.
    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/locked.db");

    SQLiteCache::Impl cache("test/fixtures/database/locked.db");

    // Then, lock the file and try again.
    FileLock guard("test/fixtures/database/locked.db");

    {
        // Adds a file.
        Log::setObserver(std::make_unique<FixtureLogObserver>());

        auto response = std::make_shared<Response>();
        response->data = "Demo";
        cache.put({ Resource::Unknown, "mapbox://test" }, response);
        cache.get({ Resource::Unknown, "mapbox://test" }, [] (std::unique_ptr<Response> res) {
            EXPECT_EQ(nullptr, res.get());
        });

        auto observer = Log::removeObserver();
        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
        EXPECT_EQ(4ul, flo->count({ EventSeverity::Error, Event::Database, 5, "database is locked" }));
    }

    {
        // Then, try to refresh it.
        Log::setObserver(std::make_unique<FixtureLogObserver>());

        auto response = std::make_shared<Response>();
        response->data = "Demo";
        cache.refresh({ Resource::Unknown, "mapbox://test" }, response->expires);
        cache.get({ Resource::Unknown, "mapbox://test" }, [] (std::unique_ptr<Response> res) {
            EXPECT_EQ(nullptr, res.get());
        });

        // Make sure that we got the right errors.
        auto observer = Log::removeObserver();
        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
        EXPECT_EQ(4ul, flo->count({ EventSeverity::Error, Event::Database, 5, "database is locked" }));
    }
}



TEST_F(Storage, DatabaseDeleted) {
    using namespace mbgl;

    // Create a locked file.
    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/locked.db");

    SQLiteCache::Impl cache("test/fixtures/database/locked.db");

    {
        // Adds a file.
        Log::setObserver(std::make_unique<FixtureLogObserver>());

        auto response = std::make_shared<Response>();
        response->data = "Demo";
        cache.put({ Resource::Unknown, "mapbox://test" }, response);
        cache.get({ Resource::Unknown, "mapbox://test" }, [] (std::unique_ptr<Response> res) {
            EXPECT_NE(nullptr, res.get());
            EXPECT_EQ("Demo", res->data);
        });

        Log::removeObserver();
    }

    deleteFile("test/fixtures/database/locked.db");

    {
        // Adds a file.
        Log::setObserver(std::make_unique<FixtureLogObserver>());

        auto response = std::make_shared<Response>();
        response->data = "Demo";
        cache.put({ Resource::Unknown, "mapbox://test" }, response);
        cache.get({ Resource::Unknown, "mapbox://test" }, [] (std::unique_ptr<Response> res) {
            EXPECT_NE(nullptr, res.get());
            EXPECT_EQ("Demo", res->data);
        });

        auto observer = Log::removeObserver();
        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
        EXPECT_EQ(1ul, flo->count({ EventSeverity::Error, Event::Database, 8, "attempt to write a readonly database" }));
    }
}



TEST_F(Storage, DatabaseInvalid) {
    using namespace mbgl;

    // Create a locked file.
    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/invalid.db");
    writeFile("test/fixtures/database/invalid.db", "this is an invalid file");

    SQLiteCache::Impl cache("test/fixtures/database/invalid.db");

    {
        // Adds a file.
        Log::setObserver(std::make_unique<FixtureLogObserver>());

        auto response = std::make_shared<Response>();
        response->data = "Demo";
        cache.put({ Resource::Unknown, "mapbox://test" }, response);
        cache.get({ Resource::Unknown, "mapbox://test" }, [] (std::unique_ptr<Response> res) {
            EXPECT_NE(nullptr, res.get());
            EXPECT_EQ("Demo", res->data);
        });

        auto observer = Log::removeObserver();
        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
        EXPECT_EQ(1ul, flo->count({ EventSeverity::Warning, Event::Database, -1, "Trashing invalid database" }));
    }
}
