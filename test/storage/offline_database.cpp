#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/io.hpp>

#include <gtest/gtest.h>
#include <sqlite3.h>

namespace {

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

}

//TEST(OfflineDatabase, NonexistentDirectory) {
//    using namespace mbgl;
//
//    Log::setObserver(std::make_unique<FixtureLogObserver>());
//
//    OfflineDatabase db("test/fixtures/404/offline.db");
//
//    db.get({ Resource::Unknown, "mapbox://test" }, [] (optional<Response> res) {
//        EXPECT_FALSE(bool(res));
//    });
//
//    auto observer = Log::removeObserver();
//    EXPECT_EQ(1ul, dynamic_cast<FixtureLogObserver*>(observer.get())->count({ EventSeverity::Error, Event::Database, 14, "unable to open database file" }));
//}

TEST(OfflineDatabase, Create) {
    using namespace mbgl;

    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/offline.db");

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    OfflineDatabase db("test/fixtures/database/offline.db");
    EXPECT_FALSE(bool(db.get({ Resource::Unknown, "mapbox://test" })));

    Log::removeObserver();
}

TEST(OfflineDatabase, SchemaVersion) {
    using namespace mbgl;

    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/offline.db");
    std::string path("test/fixtures/database/offline.db");

    {
        sqlite3* db;
        sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
        sqlite3_exec(db, "PRAGMA user_version = 1", nullptr, nullptr, nullptr);
        sqlite3_close_v2(db);
    }

    Log::setObserver(std::make_unique<FixtureLogObserver>());
    OfflineDatabase db(path);

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    EXPECT_EQ(1ul, flo->count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));
}

TEST(OfflineDatabase, Invalid) {
    using namespace mbgl;

    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/invalid.db");
    writeFile("test/fixtures/database/invalid.db", "this is an invalid file");

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    OfflineDatabase db("test/fixtures/database/invalid.db");

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    EXPECT_EQ(1ul, flo->count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));
}

//TEST(OfflineDatabase, DatabaseLockedRead) {
//    using namespace mbgl;
//
//    // Create a locked file.
//    createDir("test/fixtures/database");
//    deleteFile("test/fixtures/database/locked.db");
//    FileLock guard("test/fixtures/database/locked.db");
//
//    OfflineDatabase db("test/fixtures/database/locked.db");
//
//    {
//        // First request should fail.
//        Log::setObserver(std::make_unique<FixtureLogObserver>());
//
//        db.get({ Resource::Unknown, "mapbox://test" }, [] (optional<Response> res) {
//            EXPECT_FALSE(bool(res));
//        });
//
//        // Make sure that we got a few "database locked" errors
//        auto observer = Log::removeObserver();
//        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
//        EXPECT_EQ(4ul, flo->count({ EventSeverity::Error, Event::Database, 5, "database is locked" }));
//    }
//
//    // Then, unlock the file and try again.
//    guard.unlock();
//
//    {
//        // First, try getting a file (the cache value should not exist).
//        Log::setObserver(std::make_unique<FixtureLogObserver>());
//
//        db.get({ Resource::Unknown, "mapbox://test" }, [] (optional<Response> res) {
//            EXPECT_FALSE(bool(res));
//        });
//
//        // Make sure that we got a no errors
//        Log::removeObserver();
//    }
//}
//
//TEST(OfflineDatabase, DatabaseLockedWrite) {
//    using namespace mbgl;
//
//    // Create a locked file.
//    createDir("test/fixtures/database");
//    deleteFile("test/fixtures/database/locked.db");
//    FileLock guard("test/fixtures/database/locked.db");
//
//    OfflineDatabase db("test/fixtures/database/locked.db");
//
//    {
//        // Adds a file (which should fail).
//        Log::setObserver(std::make_unique<FixtureLogObserver>());
//
//        db.put({ Resource::Unknown, "mapbox://test" }, Response());
//        db.get({ Resource::Unknown, "mapbox://test" }, [] (optional<Response> res) {
//            EXPECT_FALSE(bool(res));
//        });
//
//        auto observer = Log::removeObserver();
//        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
//        EXPECT_EQ(8ul, flo->count({ EventSeverity::Error, Event::Database, 5, "database is locked" }));
//    }
//
//    // Then, unlock the file and try again.
//    guard.unlock();
//
//    {
//        // Then, set a file and obtain it again.
//        Log::setObserver(std::make_unique<FixtureLogObserver>());
//
//        Response response;
//        response.data = std::make_shared<std::string>("Demo");
//        db.put({ Resource::Unknown, "mapbox://test" }, response);
//        db.get({ Resource::Unknown, "mapbox://test" }, [] (optional<Response> res) {
//            ASSERT_TRUE(bool(res));
//            ASSERT_TRUE(res->data.get());
//            EXPECT_EQ("Demo", *res->data);
//        });
//
//        // Make sure that we got a no errors
//        Log::removeObserver();
//    }
//}
//
//TEST(OfflineDatabase, DatabaseDeleted) {
//    using namespace mbgl;
//
//    // Create a locked file.
//    createDir("test/fixtures/database");
//    deleteFile("test/fixtures/database/locked.db");
//
//    OfflineDatabase db("test/fixtures/database/locked.db");
//
//    {
//        // Adds a file.
//        Log::setObserver(std::make_unique<FixtureLogObserver>());
//
//        Response response;
//        response.data = std::make_shared<std::string>("Demo");
//        db.put({ Resource::Unknown, "mapbox://test" }, response);
//        db.get({ Resource::Unknown, "mapbox://test" }, [] (optional<Response> res) {
//            ASSERT_TRUE(bool(res));
//            ASSERT_TRUE(res->data.get());
//            EXPECT_EQ("Demo", *res->data);
//        });
//
//        Log::removeObserver();
//    }
//
//    deleteFile("test/fixtures/database/locked.db");
//
//    {
//        // Adds a file.
//        Log::setObserver(std::make_unique<FixtureLogObserver>());
//
//        Response response;
//        response.data = std::make_shared<std::string>("Demo");
//        db.put({ Resource::Unknown, "mapbox://test" }, response);
//        db.get({ Resource::Unknown, "mapbox://test" }, [] (optional<Response> res) {
//            ASSERT_TRUE(bool(res));
//            ASSERT_TRUE(res->data.get());
//            EXPECT_EQ("Demo", *res->data);
//        });
//
//        auto observer = Log::removeObserver();
//        auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
//        EXPECT_EQ(1ul, flo->count({ EventSeverity::Error, Event::Database, 8, "attempt to write a readonly database" }));
//    }
//}

TEST(OfflineDatabase, PutDoesNotStoreConnectionErrors) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");

    Resource resource { Resource::Unknown, "http://example.com/" };
    Response response;
    response.error = std::make_unique<Response::Error>(Response::Error::Reason::Connection);

    db.put(resource, response);
    EXPECT_FALSE(bool(db.get(resource)));
}

TEST(OfflineDatabase, PutDoesNotStoreServerErrors) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");

    Resource resource { Resource::Unknown, "http://example.com/" };
    Response response;
    response.error = std::make_unique<Response::Error>(Response::Error::Reason::Server);

    db.put(resource, response);
    EXPECT_FALSE(bool(db.get(resource)));
}

TEST(OfflineDatabase, PutResource) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");

    Resource resource { Resource::Style, "http://example.com/" };
    Response response;
    response.data = std::make_shared<std::string>("data");

    db.put(resource, response);
    auto res = db.get(resource);
    EXPECT_EQ(nullptr, res->error.get());
    EXPECT_EQ("data", *res->data);
}

TEST(OfflineDatabase, PutTile) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");

    Resource resource { Resource::Tile, "http://example.com/" };
    resource.tileData = Resource::TileData {
        "http://example.com/",
        1,
        0,
        0,
        0
    };
    Response response;
    response.data = std::make_shared<std::string>("data");

    db.put(resource, response);
    auto res = db.get(resource);
    EXPECT_EQ(nullptr, res->error.get());
    EXPECT_EQ("data", *res->data);
}

TEST(OfflineDatabase, PutResourceNotFound) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");

    Resource resource { Resource::Style, "http://example.com/" };
    Response response;
    response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);

    db.put(resource, response);
    auto res = db.get(resource);
    EXPECT_NE(nullptr, res->error);
    EXPECT_EQ(Response::Error::Reason::NotFound, res->error->reason);
    EXPECT_FALSE(res->data.get());
}

TEST(OfflineDatabase, PutTileNotFound) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");

    Resource resource { Resource::Tile, "http://example.com/" };
    resource.tileData = Resource::TileData {
        "http://example.com/",
        1,
        0,
        0,
        0
    };
    Response response;
    response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);

    db.put(resource, response);
    auto res = db.get(resource);
    EXPECT_NE(nullptr, res->error);
    EXPECT_EQ(Response::Error::Reason::NotFound, res->error->reason);
    EXPECT_FALSE(res->data.get());
}
