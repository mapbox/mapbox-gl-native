#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/string.hpp>

#include <gtest/gtest.h>
#include <sqlite3.h>
#include <thread>

using namespace std::literals::string_literals;

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

TEST(OfflineDatabase, PutResourceNoContent) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");

    Resource resource { Resource::Style, "http://example.com/" };
    Response response;
    response.noContent = true;

    db.put(resource, response);
    auto res = db.get(resource);
    EXPECT_EQ(nullptr, res->error);
    EXPECT_TRUE(res->noContent);
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
    response.noContent = true;

    db.put(resource, response);
    auto res = db.get(resource);
    EXPECT_EQ(nullptr, res->error);
    EXPECT_TRUE(res->noContent);
    EXPECT_FALSE(res->data.get());
}

TEST(OfflineDatabase, CreateRegion) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");
    OfflineRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0 };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};
    OfflineRegion region = db.createRegion(definition, metadata);

    EXPECT_EQ(definition.styleURL, region.getDefinition().styleURL);
    EXPECT_EQ(definition.bounds, region.getDefinition().bounds);
    EXPECT_EQ(definition.minZoom, region.getDefinition().minZoom);
    EXPECT_EQ(definition.maxZoom, region.getDefinition().maxZoom);
    EXPECT_EQ(definition.pixelRatio, region.getDefinition().pixelRatio);
    EXPECT_EQ(metadata, region.getMetadata());
}

TEST(OfflineDatabase, ListRegions) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");
    OfflineRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0 };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};

    OfflineRegion region = db.createRegion(definition, metadata);
    std::vector<OfflineRegion> regions = db.listRegions();

    ASSERT_EQ(1, regions.size());
    EXPECT_EQ(region.getID(), regions.at(0).getID());
    EXPECT_EQ(definition.styleURL, regions.at(0).getDefinition().styleURL);
    EXPECT_EQ(definition.bounds, regions.at(0).getDefinition().bounds);
    EXPECT_EQ(definition.minZoom, regions.at(0).getDefinition().minZoom);
    EXPECT_EQ(definition.maxZoom, regions.at(0).getDefinition().maxZoom);
    EXPECT_EQ(definition.pixelRatio, regions.at(0).getDefinition().pixelRatio);
    EXPECT_EQ(metadata, regions.at(0).getMetadata());
}

TEST(OfflineDatabase, GetRegionDefinition) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");
    OfflineRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0 };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};

    OfflineRegion region = db.createRegion(definition, metadata);
    OfflineRegionDefinition result = db.getRegionDefinition(region.getID());

    EXPECT_EQ(definition.styleURL, result.styleURL);
    EXPECT_EQ(definition.bounds, result.bounds);
    EXPECT_EQ(definition.minZoom, result.minZoom);
    EXPECT_EQ(definition.maxZoom, result.maxZoom);
    EXPECT_EQ(definition.pixelRatio, result.pixelRatio);
}

TEST(OfflineDatabase, DeleteRegion) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");
    OfflineRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0 };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};
    db.deleteRegion(db.createRegion(definition, metadata));

    ASSERT_EQ(0, db.listRegions().size());
}

TEST(OfflineDatabase, CreateRegionInfiniteMaxZoom) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");
    OfflineRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0 };
    OfflineRegionMetadata metadata;
    OfflineRegion region = db.createRegion(definition, metadata);

    EXPECT_EQ(0, region.getDefinition().minZoom);
    EXPECT_EQ(INFINITY, region.getDefinition().maxZoom);
}

TEST(OfflineDatabase, ConcurrentUse) {
    using namespace mbgl;

    createDir("test/fixtures/database");
    deleteFile("test/fixtures/database/offline.db");

    OfflineDatabase db1("test/fixtures/database/offline.db");
    OfflineDatabase db2("test/fixtures/database/offline.db");

    Resource resource { Resource::Style, "http://example.com/" };
    Response response;
    response.noContent = true;

    std::thread thread1([&] {
        for (auto i = 0; i < 100; i++) {
            db1.put(resource, response);
            EXPECT_TRUE(bool(db1.get(resource)));
        }
    });

    std::thread thread2([&] {
        for (auto i = 0; i < 100; i++) {
            db2.put(resource, response);
            EXPECT_TRUE(bool(db2.get(resource)));
        }
    });

    thread1.join();
    thread2.join();
}

TEST(OfflineDatabase, PutIgnoresOversizedResources) {
    using namespace mbgl;

    Log::setObserver(std::make_unique<FixtureLogObserver>());
    OfflineDatabase db(":memory:", 1000, 1);

    Resource resource = Resource::style("http://example.com/");
    Response response;
    response.data = std::make_shared<std::string>("data");

    db.put(resource, response);
    EXPECT_FALSE(bool(db.get(resource)));

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    EXPECT_EQ(1ul, flo->count({ EventSeverity::Warning, Event::Database, -1, "Entry too big for caching" }));
}

TEST(OfflineDatabase, PutRegionResourceDoesNotIgnoreOversizedResources) {
    using namespace mbgl;

    OfflineDatabase db(":memory:", 1000, 1);

    OfflineRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0 };
    OfflineRegion region = db.createRegion(definition, OfflineRegionMetadata());

    Resource resource = Resource::style("http://example.com/");
    Response response;
    response.data = std::make_shared<std::string>("data");

    db.putRegionResource(region.getID(), resource, response);
    EXPECT_TRUE(bool(db.get(resource)));
}

TEST(OfflineDatabase, PutEvictsLeastRecentlyUsedResources) {
    using namespace mbgl;

    OfflineDatabase db(":memory:", 1024 * 20);

    Response response;
    response.data = std::make_shared<std::string>(1024, '0');

    for (uint32_t i = 1; i <= 20; i++) {
        db.put(Resource::style("http://example.com/"s + util::toString(i)), response);
    }

    EXPECT_FALSE(bool(db.get(Resource::style("http://example.com/1"))));
    EXPECT_TRUE(bool(db.get(Resource::style("http://example.com/20"))));
}

TEST(OfflineDatabase, PutRegionResourceDoesNotEvict) {
    using namespace mbgl;

    OfflineDatabase db(":memory:", 1024 * 20);

    OfflineRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0 };
    OfflineRegion region = db.createRegion(definition, OfflineRegionMetadata());

    Response response;
    response.data = std::make_shared<std::string>(1024, '0');

    for (uint32_t i = 1; i <= 20; i++) {
        db.putRegionResource(region.getID(), Resource::style("http://example.com/"s + util::toString(i)), response);
    }

    EXPECT_TRUE(bool(db.get(Resource::style("http://example.com/1"))));
    EXPECT_TRUE(bool(db.get(Resource::style("http://example.com/20"))));
}
