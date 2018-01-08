#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/string.hpp>

#include <gtest/gtest.h>
#include <sqlite3.hpp>
#include <thread>
#include <random>

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

} // namespace

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(Create)) {
    using namespace mbgl;

    createDir("test/fixtures/offline_database");
    deleteFile("test/fixtures/offline_database/offline.db");

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    OfflineDatabase db("test/fixtures/offline_database/offline.db");
    EXPECT_FALSE(bool(db.get({ Resource::Unknown, "mapbox://test" })));

    Log::removeObserver();
}

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(SchemaVersion)) {
    using namespace mbgl;

    createDir("test/fixtures/offline_database");
    deleteFile("test/fixtures/offline_database/offline.db");
    std::string path("test/fixtures/offline_database/offline.db");

    {
        mapbox::sqlite::Database db(path, mapbox::sqlite::Create | mapbox::sqlite::ReadWrite);
        db.exec("PRAGMA user_version = 1");
    }

    Log::setObserver(std::make_unique<FixtureLogObserver>());
    OfflineDatabase db(path);

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    EXPECT_EQ(1u, flo->count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));
}

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(Invalid)) {
    using namespace mbgl;

    createDir("test/fixtures/offline_database");
    deleteFile("test/fixtures/offline_database/invalid.db");
    writeFile("test/fixtures/offline_database/invalid.db", "this is an invalid file");

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    OfflineDatabase db("test/fixtures/offline_database/invalid.db");

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    EXPECT_EQ(1u, flo->count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));
}

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

    response.data = std::make_shared<std::string>("first");
    auto insertPutResult = db.put(resource, response);
    EXPECT_TRUE(insertPutResult.first);
    EXPECT_EQ(5u, insertPutResult.second);

    auto insertGetResult = db.get(resource);
    EXPECT_EQ(nullptr, insertGetResult->error.get());
    EXPECT_EQ("first", *insertGetResult->data);

    response.data = std::make_shared<std::string>("second");
    auto updatePutResult = db.put(resource, response);
    EXPECT_FALSE(updatePutResult.first);
    EXPECT_EQ(6u, updatePutResult.second);

    auto updateGetResult = db.get(resource);
    EXPECT_EQ(nullptr, updateGetResult->error.get());
    EXPECT_EQ("second", *updateGetResult->data);
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

    response.data = std::make_shared<std::string>("first");
    auto insertPutResult = db.put(resource, response);
    EXPECT_TRUE(insertPutResult.first);
    EXPECT_EQ(5u, insertPutResult.second);

    auto insertGetResult = db.get(resource);
    EXPECT_EQ(nullptr, insertGetResult->error.get());
    EXPECT_EQ("first", *insertGetResult->data);

    response.data = std::make_shared<std::string>("second");
    auto updatePutResult = db.put(resource, response);
    EXPECT_FALSE(updatePutResult.first);
    EXPECT_EQ(6u, updatePutResult.second);

    auto updateGetResult = db.get(resource);
    EXPECT_EQ(nullptr, updateGetResult->error.get());
    EXPECT_EQ("second", *updateGetResult->data);
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

TEST(OfflineDatabase, UpdateMetadata) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");
    OfflineRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0 };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};
    OfflineRegion region = db.createRegion(definition, metadata);

    OfflineRegionMetadata newmetadata {{ 4, 5, 6 }};
    db.updateMetadata(region.getID(), newmetadata);
    EXPECT_EQ(db.listRegions().at(0).getMetadata(), newmetadata);
}

TEST(OfflineDatabase, ListRegions) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");
    OfflineRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0 };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};

    OfflineRegion region = db.createRegion(definition, metadata);
    std::vector<OfflineRegion> regions = db.listRegions();

    ASSERT_EQ(1u, regions.size());
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
    OfflineRegion region = db.createRegion(definition, metadata);

    Response response;
    response.noContent = true;

    db.putRegionResource(region.getID(), Resource::style("http://example.com/"), response);
    db.putRegionResource(region.getID(), Resource::tile("http://example.com/", 1.0, 0, 0, 0, Tileset::Scheme::XYZ), response);

    db.deleteRegion(std::move(region));

    ASSERT_EQ(0u, db.listRegions().size());
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

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(ConcurrentUse)) {
    using namespace mbgl;

    createDir("test/fixtures/offline_database");
    deleteFile("test/fixtures/offline_database/offline.db");

    OfflineDatabase db1("test/fixtures/offline_database/offline.db");
    OfflineDatabase db2("test/fixtures/offline_database/offline.db");

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

static std::shared_ptr<std::string> randomString(size_t size) {
    auto result = std::make_shared<std::string>(size, 0);
    std::mt19937 random;

    for (size_t i = 0; i < size; i++) {
        (*result)[i] = random();
    }

    return result;
}

TEST(OfflineDatabase, PutReturnsSize) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");

    Response compressible;
    compressible.data = std::make_shared<std::string>(1024, 0);
    EXPECT_EQ(17u, db.put(Resource::style("http://example.com/compressible"), compressible).second);

    Response incompressible;
    incompressible.data = randomString(1024);
    EXPECT_EQ(1024u, db.put(Resource::style("http://example.com/incompressible"), incompressible).second);

    Response noContent;
    noContent.noContent = true;
    EXPECT_EQ(0u, db.put(Resource::style("http://example.com/noContent"), noContent).second);
}

TEST(OfflineDatabase, PutEvictsLeastRecentlyUsedResources) {
    using namespace mbgl;

    OfflineDatabase db(":memory:", 1024 * 100);

    Response response;
    response.data = randomString(1024);

    for (uint32_t i = 1; i <= 100; i++) {
        Resource resource = Resource::style("http://example.com/"s + util::toString(i));
        db.put(resource, response);
        EXPECT_TRUE(bool(db.get(resource))) << i;
    }

    EXPECT_FALSE(bool(db.get(Resource::style("http://example.com/1"))));
}

TEST(OfflineDatabase, PutRegionResourceDoesNotEvict) {
    using namespace mbgl;

    OfflineDatabase db(":memory:", 1024 * 100);
    OfflineRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0 };
    OfflineRegion region = db.createRegion(definition, OfflineRegionMetadata());

    Response response;
    response.data = randomString(1024);

    for (uint32_t i = 1; i <= 100; i++) {
        db.putRegionResource(region.getID(), Resource::style("http://example.com/"s + util::toString(i)), response);
    }

    EXPECT_TRUE(bool(db.get(Resource::style("http://example.com/1"))));
    EXPECT_TRUE(bool(db.get(Resource::style("http://example.com/20"))));
}

TEST(OfflineDatabase, PutFailsWhenEvictionInsuffices) {
    using namespace mbgl;

    OfflineDatabase db(":memory:", 1024 * 100);

    Response big;
    big.data = randomString(1024 * 100);

    EXPECT_FALSE(db.put(Resource::style("http://example.com/big"), big).first);
    EXPECT_FALSE(bool(db.get(Resource::style("http://example.com/big"))));
}

TEST(OfflineDatabase, GetRegionCompletedStatus) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");
    OfflineRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0 };
    OfflineRegionMetadata metadata;
    OfflineRegion region = db.createRegion(definition, metadata);

    OfflineRegionStatus status1 = db.getRegionCompletedStatus(region.getID());
    EXPECT_EQ(0u, status1.completedResourceCount);
    EXPECT_EQ(0u, status1.completedResourceSize);
    EXPECT_EQ(0u, status1.completedTileCount);
    EXPECT_EQ(0u, status1.completedTileSize);

    Response response;
    response.data = std::make_shared<std::string>("data");

    uint64_t styleSize = db.putRegionResource(region.getID(), Resource::style("http://example.com/"), response);

    OfflineRegionStatus status2 = db.getRegionCompletedStatus(region.getID());
    EXPECT_EQ(1u, status2.completedResourceCount);
    EXPECT_EQ(styleSize, status2.completedResourceSize);
    EXPECT_EQ(0u, status2.completedTileCount);
    EXPECT_EQ(0u, status2.completedTileSize);

    uint64_t tileSize = db.putRegionResource(region.getID(), Resource::tile("http://example.com/", 1.0, 0, 0, 0, Tileset::Scheme::XYZ), response);

    OfflineRegionStatus status3 = db.getRegionCompletedStatus(region.getID());
    EXPECT_EQ(2u, status3.completedResourceCount);
    EXPECT_EQ(styleSize + tileSize, status3.completedResourceSize);
    EXPECT_EQ(1u, status3.completedTileCount);
    EXPECT_EQ(tileSize, status3.completedTileSize);
}

TEST(OfflineDatabase, HasRegionResource) {
    using namespace mbgl;

    OfflineDatabase db(":memory:", 1024 * 100);
    OfflineRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0 };
    OfflineRegion region = db.createRegion(definition, OfflineRegionMetadata());

    EXPECT_FALSE(bool(db.hasRegionResource(region.getID(), Resource::style("http://example.com/1"))));
    EXPECT_FALSE(bool(db.hasRegionResource(region.getID(), Resource::style("http://example.com/20"))));

    Response response;
    response.data = randomString(1024);

    for (uint32_t i = 1; i <= 100; i++) {
        db.putRegionResource(region.getID(), Resource::style("http://example.com/"s + util::toString(i)), response);
    }

    EXPECT_TRUE(bool(db.hasRegionResource(region.getID(), Resource::style("http://example.com/1"))));
    EXPECT_TRUE(bool(db.hasRegionResource(region.getID(), Resource::style("http://example.com/20"))));
    EXPECT_EQ(1024, *(db.hasRegionResource(region.getID(), Resource::style("http://example.com/20"))));
}

TEST(OfflineDatabase, HasRegionResourceTile) {
    using namespace mbgl;

    OfflineDatabase db(":memory:", 1024 * 100);
    OfflineRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0 };
    OfflineRegion region = db.createRegion(definition, OfflineRegionMetadata());

    Resource resource { Resource::Tile, "http://example.com/" };
    resource.tileData = Resource::TileData {
        "http://example.com/",
        1,
        0,
        0,
        0
    };
    Response response;

    response.data = std::make_shared<std::string>("first");

    EXPECT_FALSE(bool(db.hasRegionResource(region.getID(), resource)));
    db.putRegionResource(region.getID(), resource, response);
    EXPECT_TRUE(bool(db.hasRegionResource(region.getID(), resource)));
    EXPECT_EQ(5, *(db.hasRegionResource(region.getID(), resource)));

    OfflineRegion anotherRegion = db.createRegion(definition, OfflineRegionMetadata());
    EXPECT_LT(region.getID(), anotherRegion.getID());
    EXPECT_TRUE(bool(db.hasRegionResource(anotherRegion.getID(), resource)));
    EXPECT_EQ(5, *(db.hasRegionResource(anotherRegion.getID(), resource)));

}

TEST(OfflineDatabase, OfflineMapboxTileCount) {
    using namespace mbgl;

    OfflineDatabase db(":memory:");
    OfflineRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0 };
    OfflineRegionMetadata metadata;

    OfflineRegion region1 = db.createRegion(definition, metadata);
    OfflineRegion region2 = db.createRegion(definition, metadata);

    Resource nonMapboxTile = Resource::tile("http://example.com/", 1.0, 0, 0, 0, Tileset::Scheme::XYZ);
    Resource mapboxTile1 = Resource::tile("mapbox://tiles/1", 1.0, 0, 0, 0, Tileset::Scheme::XYZ);
    Resource mapboxTile2 = Resource::tile("mapbox://tiles/2", 1.0, 0, 0, 1, Tileset::Scheme::XYZ);

    Response response;
    response.data = std::make_shared<std::string>("data");

    // Count is initially zero.
    EXPECT_EQ(0u, db.getOfflineMapboxTileCount());

    // Count stays the same after putting a non-tile resource.
    db.putRegionResource(region1.getID(), Resource::style("http://example.com/"), response);
    EXPECT_EQ(0u, db.getOfflineMapboxTileCount());

    // Count stays the same after putting a non-Mapbox tile.
    db.putRegionResource(region1.getID(), nonMapboxTile, response);
    EXPECT_EQ(0u, db.getOfflineMapboxTileCount());

    // Count increases after putting a Mapbox tile not used by another region.
    db.putRegionResource(region1.getID(), mapboxTile1, response);
    EXPECT_EQ(1u, db.getOfflineMapboxTileCount());

    // Count stays the same after putting a Mapbox tile used by another region.
    db.putRegionResource(region2.getID(), mapboxTile1, response);
    EXPECT_EQ(1u, db.getOfflineMapboxTileCount());

    // Count stays the same after putting a Mapbox tile used by the same region.
    db.putRegionResource(region2.getID(), mapboxTile1, response);
    EXPECT_EQ(1u, db.getOfflineMapboxTileCount());

    // Count stays the same after deleting a region when the tile is still used by another region.
    db.deleteRegion(std::move(region2));
    EXPECT_EQ(1u, db.getOfflineMapboxTileCount());

    // Count stays the same after the putting a non-offline Mapbox tile.
    db.put(mapboxTile2, response);
    EXPECT_EQ(1u, db.getOfflineMapboxTileCount());

    // Count increases after putting a pre-existing, but non-offline Mapbox tile.
    db.putRegionResource(region1.getID(), mapboxTile2, response);
    EXPECT_EQ(2u, db.getOfflineMapboxTileCount());

    // Count decreases after deleting a region when the tiles are not used by other regions.
    db.deleteRegion(std::move(region1));
    EXPECT_EQ(0u, db.getOfflineMapboxTileCount());
}

static int databasePageCount(const std::string& path) {
    mapbox::sqlite::Database db(path, mapbox::sqlite::ReadOnly);
    mapbox::sqlite::Statement stmt = db.prepare("pragma page_count");
    stmt.run();
    return stmt.get<int>(0);
}

static int databaseUserVersion(const std::string& path) {
    mapbox::sqlite::Database db(path, mapbox::sqlite::ReadOnly);
    mapbox::sqlite::Statement stmt = db.prepare("pragma user_version");
    stmt.run();
    return stmt.get<int>(0);
}

static std::string databaseJournalMode(const std::string& path) {
    mapbox::sqlite::Database db(path, mapbox::sqlite::ReadOnly);
    mapbox::sqlite::Statement stmt = db.prepare("pragma journal_mode");
    stmt.run();
    return stmt.get<std::string>(0);
}

static int databaseSyncMode(const std::string& path) {
    mapbox::sqlite::Database db(path, mapbox::sqlite::ReadOnly);
    mapbox::sqlite::Statement stmt = db.prepare("pragma synchronous");
    stmt.run();
    return stmt.get<int>(0);
}

static std::vector<std::string> databaseTableColumns(const std::string& path, const std::string& name) {
    mapbox::sqlite::Database db(path, mapbox::sqlite::ReadOnly);
    const auto sql = std::string("pragma table_info(") + name + ")";
    mapbox::sqlite::Statement stmt = db.prepare(sql.c_str());
    std::vector<std::string> columns;
    while (stmt.run()) {
        columns.push_back(stmt.get<std::string>(1));
    }
    return columns;
}

TEST(OfflineDatabase, MigrateFromV2Schema) {
    using namespace mbgl;

    // v2.db is a v2 database containing a single offline region with a small number of resources.

    deleteFile("test/fixtures/offline_database/migrated.db");
    writeFile("test/fixtures/offline_database/migrated.db", util::read_file("test/fixtures/offline_database/v2.db"));

    {
        OfflineDatabase db("test/fixtures/offline_database/migrated.db", 0);
        auto regions = db.listRegions();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion("test/fixtures/offline_database/migrated.db"));
    EXPECT_LT(databasePageCount("test/fixtures/offline_database/migrated.db"),
              databasePageCount("test/fixtures/offline_database/v2.db"));
}

TEST(OfflineDatabase, MigrateFromV3Schema) {
    using namespace mbgl;

    // v3.db is a v3 database, migrated from v2.

    deleteFile("test/fixtures/offline_database/migrated.db");
    writeFile("test/fixtures/offline_database/migrated.db", util::read_file("test/fixtures/offline_database/v3.db"));

    {
        OfflineDatabase db("test/fixtures/offline_database/migrated.db", 0);
        auto regions = db.listRegions();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion("test/fixtures/offline_database/migrated.db"));
}

TEST(OfflineDatabase, MigrateFromV4Schema) {
    using namespace mbgl;

    // v4.db is a v4 database, migrated from v2 & v3. This database used `journal_mode = WAL` and `synchronous = NORMAL`.

    deleteFile("test/fixtures/offline_database/migrated.db");
    writeFile("test/fixtures/offline_database/migrated.db", util::read_file("test/fixtures/offline_database/v4.db"));

    {
        OfflineDatabase db("test/fixtures/offline_database/migrated.db", 0);
        auto regions = db.listRegions();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion("test/fixtures/offline_database/migrated.db"));

    // Journal mode should be DELETE after migration to v5.
    EXPECT_EQ("delete", databaseJournalMode("test/fixtures/offline_database/migrated.db"));

    // Synchronous setting should be FULL (2) after migration to v5.
    EXPECT_EQ(2, databaseSyncMode("test/fixtures/offline_database/migrated.db"));
}


TEST(OfflineDatabase, MigrateFromV5Schema) {
    using namespace mbgl;

    // v5.db is a v5 database, migrated from v2, v3 & v4.

    deleteFile("test/fixtures/offline_database/migrated.db");
    writeFile("test/fixtures/offline_database/migrated.db", util::read_file("test/fixtures/offline_database/v5.db"));

    {
        OfflineDatabase db("test/fixtures/offline_database/migrated.db", 0);
        auto regions = db.listRegions();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion("test/fixtures/offline_database/migrated.db"));

    EXPECT_EQ((std::vector<std::string>{ "id", "url_template", "pixel_ratio", "z", "x", "y",
                                         "expires", "modified", "etag", "data", "compressed",
                                         "accessed", "must_revalidate" }),
              databaseTableColumns("test/fixtures/offline_database/migrated.db", "tiles"));
    EXPECT_EQ((std::vector<std::string>{ "id", "url", "kind", "expires", "modified", "etag", "data",
                                         "compressed", "accessed", "must_revalidate" }),
              databaseTableColumns("test/fixtures/offline_database/migrated.db", "resources"));
}

TEST(OfflineDatabase, DowngradeSchema) {
    using namespace mbgl;

    // v999.db is a v999 database, it should be deleted
    // and recreated with the current schema.

    deleteFile("test/fixtures/offline_database/migrated.db");
    writeFile("test/fixtures/offline_database/migrated.db", util::read_file("test/fixtures/offline_database/v999.db"));

    {
        OfflineDatabase db("test/fixtures/offline_database/migrated.db", 0);
    }

    EXPECT_EQ(6, databaseUserVersion("test/fixtures/offline_database/migrated.db"));

    EXPECT_EQ((std::vector<std::string>{ "id", "url_template", "pixel_ratio", "z", "x", "y",
                                         "expires", "modified", "etag", "data", "compressed",
                                         "accessed", "must_revalidate" }),
              databaseTableColumns("test/fixtures/offline_database/migrated.db", "tiles"));
    EXPECT_EQ((std::vector<std::string>{ "id", "url", "kind", "expires", "modified", "etag", "data",
                                         "compressed", "accessed", "must_revalidate" }),
              databaseTableColumns("test/fixtures/offline_database/migrated.db", "resources"));
}
