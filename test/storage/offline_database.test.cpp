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

using namespace mbgl;

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

void copyFile(const char* orig, const char* dest) {
    util::write_file(dest, util::read_file(orig));
}

} // namespace

class OfflineDatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        log.reset();
        log = std::make_unique<FixtureLog>();
        createDir("test/fixtures/offline_database");
        deleteFile("test/fixtures/offline_database/offline.db");
    }

    void TearDown() override {
        deleteFile("test/fixtures/offline_database/offline.db");
        EXPECT_EQ(0u, log->uncheckedCount());
        log.reset();
    }

    static constexpr const char* filename = "test/fixtures/offline_database/offline.db";

    std::unique_ptr<FixtureLog> log;
};

TEST_F(OfflineDatabaseTest, TEST_REQUIRES_WRITE(Create)) {
    OfflineDatabase db(filename);
    EXPECT_FALSE(bool(db.get({ Resource::Unknown, "mapbox://test" })));

    EXPECT_EQ(1u, log->count({ EventSeverity::Error, Event::Database, -1, "cannot open file" }, true));
    EXPECT_EQ(1u, log->count({ EventSeverity::Error, Event::Database, -1, "No such file or directory" }, true));
}

TEST_F(OfflineDatabaseTest, TEST_REQUIRES_WRITE(SchemaVersion)) {
    {
        mapbox::sqlite::Database db{ filename, mapbox::sqlite::Create | mapbox::sqlite::ReadWrite };
        db.exec("PRAGMA user_version = 1");
    }

    OfflineDatabase db(filename);

    EXPECT_EQ(1u, log->count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));
}

TEST_F(OfflineDatabaseTest, TEST_REQUIRES_WRITE(Invalid)) {
    util::write_file(filename, "this is an invalid file");
    OfflineDatabase db(filename);

    EXPECT_EQ(1u, log->count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));
    EXPECT_EQ(1u, log->count({ EventSeverity::Error, Event::Database, -1, "file is encrypted or is not a database" }, true));
}

TEST_F(OfflineDatabaseTest, PutDoesNotStoreConnectionErrors) {
    OfflineDatabase db(":memory:");

    Resource resource { Resource::Unknown, "http://example.com/" };
    Response response;
    response.error = std::make_unique<Response::Error>(Response::Error::Reason::Connection);

    db.put(resource, response);
    EXPECT_FALSE(bool(db.get(resource)));
}

TEST_F(OfflineDatabaseTest, PutDoesNotStoreServerErrors) {
    OfflineDatabase db(":memory:");

    Resource resource { Resource::Unknown, "http://example.com/" };
    Response response;
    response.error = std::make_unique<Response::Error>(Response::Error::Reason::Server);

    db.put(resource, response);
    EXPECT_FALSE(bool(db.get(resource)));
}

TEST_F(OfflineDatabaseTest, PutResource) {
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

TEST_F(OfflineDatabaseTest, TEST_REQUIRES_WRITE(GetResourceFromOfflineRegion)) {
    copyFile("test/fixtures/offline_database/satellite_test.db", filename);
    OfflineDatabase db(filename, mapbox::sqlite::ReadOnly);

    Resource resource = Resource::style("mapbox://styles/mapbox/satellite-v9");
    ASSERT_TRUE(db.get(resource));
}

TEST_F(OfflineDatabaseTest, PutAndGetResource) {
    OfflineDatabase db(":memory:");

    Response response1;
    response1.data = std::make_shared<std::string>("foobar");

    Resource resource = Resource::style("mapbox://example.com/style");

    db.put(resource, response1);

    auto response2 = db.get(resource);

    ASSERT_EQ(*response1.data, *(*response2).data);
}

TEST_F(OfflineDatabaseTest, PutTile) {
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

TEST_F(OfflineDatabaseTest, PutResourceNoContent) {
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

TEST_F(OfflineDatabaseTest, PutTileNotFound) {
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

TEST_F(OfflineDatabaseTest, CreateRegion) {
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

TEST_F(OfflineDatabaseTest, UpdateMetadata) {
    OfflineDatabase db(":memory:");
    OfflineRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0 };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};
    OfflineRegion region = db.createRegion(definition, metadata);

    OfflineRegionMetadata newmetadata {{ 4, 5, 6 }};
    db.updateMetadata(region.getID(), newmetadata);
    EXPECT_EQ(db.listRegions().at(0).getMetadata(), newmetadata);
}

TEST_F(OfflineDatabaseTest, ListRegions) {
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

TEST_F(OfflineDatabaseTest, GetRegionDefinition) {
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

TEST_F(OfflineDatabaseTest, DeleteRegion) {
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

TEST_F(OfflineDatabaseTest, CreateRegionInfiniteMaxZoom) {
    OfflineDatabase db(":memory:");
    OfflineRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0 };
    OfflineRegionMetadata metadata;
    OfflineRegion region = db.createRegion(definition, metadata);

    EXPECT_EQ(0, region.getDefinition().minZoom);
    EXPECT_EQ(INFINITY, region.getDefinition().maxZoom);
}

TEST_F(OfflineDatabaseTest, TEST_REQUIRES_WRITE(ConcurrentUse)) {
    OfflineDatabase db1(filename);
    OfflineDatabase db2(filename);

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

    EXPECT_EQ(1u, log->count({ EventSeverity::Error, Event::Database, -1, "cannot open file" }, true));
    EXPECT_EQ(1u, log->count({ EventSeverity::Error, Event::Database, -1, "No such file or directory" }, true));
}

static std::shared_ptr<std::string> randomString(size_t size) {
    auto result = std::make_shared<std::string>(size, 0);
    std::mt19937 random;

    for (size_t i = 0; i < size; i++) {
        (*result)[i] = random();
    }

    return result;
}

TEST_F(OfflineDatabaseTest, PutReturnsSize) {
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

TEST_F(OfflineDatabaseTest, PutEvictsLeastRecentlyUsedResources) {
    OfflineDatabase db(":memory:", 1024 * 100);

    Response response;
    response.data = randomString(1024);

    for (uint32_t i = 1; i <= 100; i++) {
        Resource resource = Resource::style("http://example.com/" + util::toString(i));
        db.put(resource, response);
        EXPECT_TRUE(bool(db.get(resource))) << i;
    }

    EXPECT_FALSE(bool(db.get(Resource::style("http://example.com/1"))));
}

TEST_F(OfflineDatabaseTest, PutRegionResourceDoesNotEvict) {
    OfflineDatabase db(":memory:", 1024 * 100);
    OfflineRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0 };
    OfflineRegion region = db.createRegion(definition, OfflineRegionMetadata());

    Response response;
    response.data = randomString(1024);

    for (uint32_t i = 1; i <= 100; i++) {
        db.putRegionResource(region.getID(), Resource::style("http://example.com/" + util::toString(i)), response);
    }

    EXPECT_TRUE(bool(db.get(Resource::style("http://example.com/1"))));
    EXPECT_TRUE(bool(db.get(Resource::style("http://example.com/20"))));
}

TEST_F(OfflineDatabaseTest, PutFailsWhenEvictionInsuffices) {
    OfflineDatabase db(":memory:", 1024 * 100);

    Response big;
    big.data = randomString(1024 * 100);

    EXPECT_FALSE(db.put(Resource::style("http://example.com/big"), big).first);
    EXPECT_FALSE(bool(db.get(Resource::style("http://example.com/big"))));

    EXPECT_EQ(1u, log->count({ EventSeverity::Debug, Event::Database, -1, "Unable to make space for entry" }));
}

TEST_F(OfflineDatabaseTest, GetRegionCompletedStatus) {
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

TEST_F(OfflineDatabaseTest, HasRegionResource) {
    OfflineDatabase db(":memory:", 1024 * 100);
    OfflineRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0 };
    OfflineRegion region = db.createRegion(definition, OfflineRegionMetadata());

    EXPECT_FALSE(bool(db.hasRegionResource(region.getID(), Resource::style("http://example.com/1"))));
    EXPECT_FALSE(bool(db.hasRegionResource(region.getID(), Resource::style("http://example.com/20"))));

    Response response;
    response.data = randomString(1024);

    for (uint32_t i = 1; i <= 100; i++) {
        db.putRegionResource(region.getID(), Resource::style("http://example.com/" + util::toString(i)), response);
    }

    EXPECT_TRUE(bool(db.hasRegionResource(region.getID(), Resource::style("http://example.com/1"))));
    EXPECT_TRUE(bool(db.hasRegionResource(region.getID(), Resource::style("http://example.com/20"))));
    EXPECT_EQ(1024, *(db.hasRegionResource(region.getID(), Resource::style("http://example.com/20"))));
}

TEST_F(OfflineDatabaseTest, HasRegionResourceTile) {
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

TEST_F(OfflineDatabaseTest, OfflineMapboxTileCount) {
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
    mapbox::sqlite::Database db{ path, mapbox::sqlite::ReadOnly };
    mapbox::sqlite::Statement stmt{ db, "pragma page_count" };
    mapbox::sqlite::Query query{ stmt };
    query.run();
    return query.get<int>(0);
}

static int databaseUserVersion(const std::string& path) {
    mapbox::sqlite::Database db{ path, mapbox::sqlite::ReadOnly };
    mapbox::sqlite::Statement stmt{ db, "pragma user_version" };
    mapbox::sqlite::Query query{ stmt };
    query.run();
    return query.get<int>(0);
}

static std::string databaseJournalMode(const std::string& path) {
    mapbox::sqlite::Database db{ path, mapbox::sqlite::ReadOnly };
    mapbox::sqlite::Statement stmt{ db, "pragma journal_mode" };
    mapbox::sqlite::Query query{ stmt };
    query.run();
    return query.get<std::string>(0);
}

static int databaseSyncMode(const std::string& path) {
    mapbox::sqlite::Database db{ path, mapbox::sqlite::ReadOnly };
    mapbox::sqlite::Statement stmt{ db, "pragma synchronous" };
    mapbox::sqlite::Query query{ stmt };
    query.run();
    return query.get<int>(0);
}

static std::vector<std::string> databaseTableColumns(const std::string& path, const std::string& name) {
    mapbox::sqlite::Database db{ path, mapbox::sqlite::ReadOnly };
    const auto sql = std::string("pragma table_info(") + name + ")";
    mapbox::sqlite::Statement stmt{ db, sql.c_str() };
    mapbox::sqlite::Query query{ stmt };
    std::vector<std::string> columns;
    while (query.run()) {
        columns.push_back(query.get<std::string>(1));
    }
    return columns;
}

TEST_F(OfflineDatabaseTest, MigrateFromV2Schema) {
    // v2.db is a v2 database containing a single offline region with a small number of resources.
    copyFile("test/fixtures/offline_database/v2.db", filename);

    {
        OfflineDatabase db(filename, 0);
        auto regions = db.listRegions();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion(filename));
    EXPECT_LT(databasePageCount(filename),
              databasePageCount("test/fixtures/offline_database/v2.db"));
}

TEST_F(OfflineDatabaseTest, MigrateFromV3Schema) {
    // v3.db is a v3 database, migrated from v2.
    copyFile("test/fixtures/offline_database/v3.db", filename);

    {
        OfflineDatabase db(filename, 0);
        auto regions = db.listRegions();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion(filename));
}

TEST_F(OfflineDatabaseTest, MigrateFromV4Schema) {
    // v4.db is a v4 database, migrated from v2 & v3. This database used `journal_mode = WAL` and `synchronous = NORMAL`.
    copyFile("test/fixtures/offline_database/v4.db", filename);

    {
        OfflineDatabase db(filename, 0);
        auto regions = db.listRegions();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion(filename));

    // Journal mode should be DELETE after migration to v5.
    EXPECT_EQ("delete", databaseJournalMode(filename));

    // Synchronous setting should be FULL (2) after migration to v5.
    EXPECT_EQ(2, databaseSyncMode(filename));
}


TEST_F(OfflineDatabaseTest, MigrateFromV5Schema) {
    // v5.db is a v5 database, migrated from v2, v3 & v4.
    copyFile("test/fixtures/offline_database/v5.db", filename);

    {
        OfflineDatabase db(filename, 0);
        auto regions = db.listRegions();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion(filename));

    EXPECT_EQ((std::vector<std::string>{ "id", "url_template", "pixel_ratio", "z", "x", "y",
                                         "expires", "modified", "etag", "data", "compressed",
                                         "accessed", "must_revalidate" }),
              databaseTableColumns("test/fixtures/offline_database/migrated.db", "tiles"));
    EXPECT_EQ((std::vector<std::string>{ "id", "url", "kind", "expires", "modified", "etag", "data",
                                         "compressed", "accessed", "must_revalidate" }),
              databaseTableColumns("test/fixtures/offline_database/migrated.db", "resources"));
}

TEST_F(OfflineDatabaseTest, DowngradeSchema) {
    // v999.db is a v999 database, it should be deleted
    // and recreated with the current schema.
    copyFile("test/fixtures/offline_database/v999.db", filename);

    {
        OfflineDatabase db(filename, 0);
    }

    EXPECT_EQ(6, databaseUserVersion(filename));

    EXPECT_EQ((std::vector<std::string>{ "id", "url_template", "pixel_ratio", "z", "x", "y",
                                         "expires", "modified", "etag", "data", "compressed",
                                         "accessed", "must_revalidate" }),
              databaseTableColumns("test/fixtures/offline_database/migrated.db", "tiles"));
    EXPECT_EQ((std::vector<std::string>{ "id", "url", "kind", "expires", "modified", "etag", "data",
                                         "compressed", "accessed", "must_revalidate" }),
              databaseTableColumns("test/fixtures/offline_database/migrated.db", "resources"));

    EXPECT_EQ(1u, log->count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));
}
