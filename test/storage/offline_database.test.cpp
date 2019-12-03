#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/sqlite3_test_fs.hpp>

#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/string.hpp>

#include <mbgl/storage/sqlite3.hpp>
#include <thread>
#include <random>

using namespace std::literals::string_literals;
using namespace mbgl;
using mapbox::sqlite::ResultCode;

static constexpr const char* filename = "test/fixtures/offline_database/offline.db";
static constexpr const char* filename_sideload = "test/fixtures/offline_database/offline_sideload.db";
#ifndef __QT__ // Qt doesn't expose the ability to register virtual file system handlers.
static constexpr const char* filename_test_fs = "file:test/fixtures/offline_database/offline.db?vfs=test_fs";
#endif

static void deleteDatabaseFiles() {
    // Delete leftover journaling files as well.
    util::deleteFile(filename);
    util::deleteFile(filename + "-wal"s);
    util::deleteFile(filename + "-journal"s);
}

static std::shared_ptr<std::string> randomString(size_t size) {
    auto result = std::make_shared<std::string>(size, 0);
    std::mt19937 random;

    for (size_t i = 0; i < size; i++) {
        (*result)[i] = random();
    }

    return result;
}

static FixtureLog::Message error(ResultCode code, const char* message) {
    return { EventSeverity::Error, Event::Database, static_cast<int64_t>(code), message };
}

static __attribute__((unused)) FixtureLog::Message warning(ResultCode code, const char* message) {
    return { EventSeverity::Warning, Event::Database, static_cast<int64_t>(code), message };
}

static int databasePageCount(const std::string& path) {
    mapbox::sqlite::Database db = mapbox::sqlite::Database::open(path, mapbox::sqlite::ReadOnly);
    mapbox::sqlite::Statement stmt{ db, "pragma page_count" };
    mapbox::sqlite::Query query{ stmt };
    query.run();
    return query.get<int>(0);
}

static int databaseUserVersion(const std::string& path) {
    mapbox::sqlite::Database db = mapbox::sqlite::Database::open(path, mapbox::sqlite::ReadOnly);
    mapbox::sqlite::Statement stmt{ db, "pragma user_version" };
    mapbox::sqlite::Query query{ stmt };
    query.run();
    return query.get<int>(0);
}

static std::string databaseJournalMode(const std::string& path) {
    mapbox::sqlite::Database db = mapbox::sqlite::Database::open(path, mapbox::sqlite::ReadOnly);
    mapbox::sqlite::Statement stmt{ db, "pragma journal_mode" };
    mapbox::sqlite::Query query{ stmt };
    query.run();
    return query.get<std::string>(0);
}

static int databaseSyncMode(const std::string& path) {
    mapbox::sqlite::Database db = mapbox::sqlite::Database::open(path, mapbox::sqlite::ReadOnly);
    mapbox::sqlite::Statement stmt{ db, "pragma synchronous" };
    mapbox::sqlite::Query query{ stmt };
    query.run();
    return query.get<int>(0);
}

static std::vector<std::string> databaseTableColumns(const std::string& path, const std::string& name) {
    mapbox::sqlite::Database db = mapbox::sqlite::Database::open(path, mapbox::sqlite::ReadOnly);
    const auto sql = std::string("pragma table_info(") + name + ")";
    mapbox::sqlite::Statement stmt{ db, sql.c_str() };
    mapbox::sqlite::Query query{ stmt };
    std::vector<std::string> columns;
    while (query.run()) {
        columns.push_back(query.get<std::string>(1));
    }
    return columns;
}

static int databaseAutoVacuum(const std::string& path) {
    mapbox::sqlite::Database db = mapbox::sqlite::Database::open(path, mapbox::sqlite::ReadOnly);
    mapbox::sqlite::Statement stmt{db, "pragma auto_vacuum"};
    mapbox::sqlite::Query query{stmt};
    query.run();
    return query.get<int>(0);
}

namespace fixture {

const Resource resource{ Resource::Style, "mapbox://test" };
const Resource tile = Resource::tile("mapbox://test", 1, 0, 0, 0, Tileset::Scheme::XYZ);
const Response response = [] {
    Response res;
    res.data = std::make_shared<std::string>("first");
    return res;
}();

} // namespace fixture

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(Create)) {
    FixtureLog log;
    deleteDatabaseFiles();
    OfflineDatabase db(filename);
    EXPECT_FALSE(bool(db.get({ Resource::Unknown, "mapbox://test" })));

    EXPECT_EQ(0u, log.uncheckedCount());
}

#ifndef __QT__ // Qt doesn't expose the ability to register virtual file system handlers.
TEST(OfflineDatabase, TEST_REQUIRES_WRITE(CreateFail)) {
    FixtureLog log;
    deleteDatabaseFiles();
    test::SQLite3TestFS fs;

    // Opening the database will fail because our mock VFS returns a SQLITE_CANTOPEN error because
    // it is not allowed to create the file. The OfflineDatabase object should handle this gracefully
    // and treat it like an empty cache that can't be written to.
    fs.allowFileCreate(false);
    OfflineDatabase db(filename_test_fs);
    EXPECT_EQ(1u, log.count(warning(ResultCode::CantOpen, "Can't open database: unable to open database file")));

    EXPECT_EQ(0u, log.uncheckedCount());

    // We can try to insert things into the cache, but since the cache database isn't open, it won't be stored.
    for (const auto& res : { fixture::resource, fixture::tile }) {
        EXPECT_EQ(std::make_pair(false, uint64_t(0)), db.put(res, fixture::response));
        EXPECT_EQ(1u, log.count(warning(ResultCode::CantOpen, "Can't write resource: unable to open database file")));
        EXPECT_EQ(0u, log.uncheckedCount());
    }

    // We can also still "query" the database even though it is not open, and we will always get an empty result.
    for (const auto& res : { fixture::resource, fixture::tile }) {
        EXPECT_FALSE(bool(db.get(res)));
        EXPECT_EQ(1u, log.count(warning(ResultCode::CantOpen, "Can't update timestamp: unable to open database file")));
        EXPECT_EQ(1u, log.count(warning(ResultCode::CantOpen, "Can't read resource: unable to open database file")));
        EXPECT_EQ(0u, log.uncheckedCount());
    }

    // Now, we're "freeing up" some space on the disk, and try to insert and query again. This time, we should
    // be opening the datbase, creating the schema, and writing the data so that we can retrieve it again.
    fs.allowFileCreate(true);
    for (const auto& res : { fixture::resource, fixture::tile }) {
        EXPECT_EQ(std::make_pair(true, uint64_t(5)), db.put(res, fixture::response));
        auto result = db.get(res);
        EXPECT_EQ(0u, log.uncheckedCount());
        ASSERT_TRUE(result && result->data);
        EXPECT_EQ("first", *result->data);
    }

    // Next, set the file system to read only mode and try to read the data again. While we can't
    // write anymore, we should still be able to read, and the query that tries to update the last
    // accessed timestamp may fail without crashing.
    fs.allowFileCreate(false);
    fs.setWriteLimit(0);
    for (const auto& res : { fixture::resource, fixture::tile }) {
        auto result = db.get(res);
        EXPECT_EQ(1u, log.count(warning(ResultCode::CantOpen, "Can't update timestamp: unable to open database file")));
        EXPECT_EQ(0u, log.uncheckedCount());

        ASSERT_TRUE(result && result->data);
        EXPECT_EQ("first", *result->data);
    }
    fs.setDebug(false);

    // We're allowing SQLite to create a journal file, but restrict the number of bytes it
    // can write so that it can start writing the journal file, but eventually fails during the
    // timestamp update.
    fs.allowFileCreate(true);
    fs.setWriteLimit(8192);
    for (const auto& res : { fixture::resource, fixture::tile }) {
        auto result = db.get(res);
        EXPECT_EQ(1u, log.count(warning(ResultCode::Full, "Can't update timestamp: database or disk is full")));
        EXPECT_EQ(0u, log.uncheckedCount());
        ASSERT_TRUE(result && result->data);
        EXPECT_EQ("first", *result->data);
    }

    // Lastly, we're disabling all I/O to simulate a backgrounded app that is restricted from doing
    // any disk I/O at all.
    fs.setWriteLimit(-1);
    fs.allowIO(false);
    for (const auto& res : { fixture::resource, fixture::tile }) {
        // First, try reading.
        auto result = db.get(res);
        EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't update timestamp: authorization denied")));
        EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't read resource: authorization denied")));
        EXPECT_EQ(0u, log.uncheckedCount());
        EXPECT_FALSE(result);

        // Now try inserting.
        EXPECT_EQ(std::make_pair(false, uint64_t(0)), db.put(res, fixture::response));
        EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't write resource: authorization denied")));
        EXPECT_EQ(0u, log.uncheckedCount());
    }

    // Allow deleting the database.
    fs.reset();
}
#endif // __QT__

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(SchemaVersion)) {
    FixtureLog log;
    deleteDatabaseFiles();

    {
        mapbox::sqlite::Database db = mapbox::sqlite::Database::open(filename, mapbox::sqlite::ReadWriteCreate);
        db.setBusyTimeout(Milliseconds(1000));
        db.exec("PRAGMA user_version = 1");
    }

    {
        OfflineDatabase db(filename);
    }

    EXPECT_EQ(6, databaseUserVersion(filename));

    OfflineDatabase db(filename);
    // Now try inserting and reading back to make sure we have a valid database.
    for (const auto& res : { fixture::resource, fixture::tile }) {
        EXPECT_EQ(std::make_pair(true, uint64_t(5)), db.put(res, fixture::response));
        EXPECT_EQ(0u, log.uncheckedCount());
        auto result = db.get(res);
        EXPECT_EQ(0u, log.uncheckedCount());
        ASSERT_TRUE(result && result->data);
        EXPECT_EQ("first", *result->data);
    }

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(Invalid)) {
    FixtureLog log;
    deleteDatabaseFiles();
    util::write_file(filename, "this is an invalid file");

    OfflineDatabase db(filename);
    // Checking two possibilities for the error string because it apparently changes between SQLite versions.
    EXPECT_EQ(1u, log.count(error(ResultCode::NotADB, "Can't open database: file is encrypted or is not a database"), true) +
                  log.count(error(ResultCode::NotADB, "Can't open database: file is not a database"), true));
    EXPECT_EQ(1u, log.count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));

    // Now try inserting and reading back to make sure we have a valid database.
    for (const auto& res : { fixture::resource, fixture::tile }) {
        EXPECT_EQ(std::make_pair(true, uint64_t(5)), db.put(res, fixture::response));
        EXPECT_EQ(0u, log.uncheckedCount());
        auto result = db.get(res);
        EXPECT_EQ(0u, log.uncheckedCount());
        ASSERT_TRUE(result && result->data);
        EXPECT_EQ("first", *result->data);
    }
}

TEST(OfflineDatabase, PutDoesNotStoreConnectionErrors) {
    FixtureLog log;
    OfflineDatabase db(":memory:");

    Resource resource { Resource::Unknown, "http://example.com/" };
    Response response;
    response.error = std::make_unique<Response::Error>(Response::Error::Reason::Connection);

    db.put(resource, response);
    EXPECT_FALSE(bool(db.get(resource)));

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, PutDoesNotStoreServerErrors) {
    FixtureLog log;
    OfflineDatabase db(":memory:");

    Resource resource { Resource::Unknown, "http://example.com/" };
    Response response;
    response.error = std::make_unique<Response::Error>(Response::Error::Reason::Server);

    db.put(resource, response);
    EXPECT_FALSE(bool(db.get(resource)));

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, PutResource) {
    FixtureLog log;
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

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(GetResourceFromOfflineRegion)) {
    FixtureLog log;
    deleteDatabaseFiles();
    util::copyFile(filename, "test/fixtures/offline_database/satellite_test.db");

    OfflineDatabase db(filename);

    Resource resource = Resource::style("mapbox://styles/mapbox/satellite-v9");
    ASSERT_TRUE(db.get(resource));

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, PutAndGetResource) {
    FixtureLog log;
    OfflineDatabase db(":memory:");

    Response response1;
    response1.data = std::make_shared<std::string>("foobar");

    Resource resource = Resource::style("mapbox://example.com/style");

    db.put(resource, response1);

    auto response2 = db.get(resource);

    ASSERT_EQ(*response1.data, *(*response2).data);

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, PutTile) {
    FixtureLog log;
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

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, PutResourceNoContent) {
    FixtureLog log;
    OfflineDatabase db(":memory:");

    Resource resource { Resource::Style, "http://example.com/" };
    Response response;
    response.noContent = true;

    db.put(resource, response);
    auto res = db.get(resource);
    EXPECT_EQ(nullptr, res->error);
    EXPECT_TRUE(res->noContent);
    EXPECT_FALSE(res->data.get());

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, PutTileNotFound) {
    FixtureLog log;
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

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, CreateRegion) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    OfflineTilePyramidRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0, true };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};
    auto region = db.createRegion(definition, metadata);
    ASSERT_TRUE(region);

    EXPECT_EQ(0u, log.uncheckedCount());

    region->getDefinition().match(
            [&](OfflineTilePyramidRegionDefinition& def) {
                EXPECT_EQ(definition.styleURL, def.styleURL);
                EXPECT_EQ(definition.bounds, def.bounds);
                EXPECT_EQ(definition.minZoom, def.minZoom);
                EXPECT_EQ(definition.maxZoom, def.maxZoom);
                EXPECT_EQ(definition.pixelRatio, def.pixelRatio);
                EXPECT_EQ(definition.includeIdeographs, def.includeIdeographs);
            }, [](auto&) {
                EXPECT_FALSE(false);
            }
    );
    EXPECT_EQ(metadata, region->getMetadata());
}

TEST(OfflineDatabase, UpdateMetadata) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    OfflineTilePyramidRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0, true };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};
    auto region = db.createRegion(definition, metadata);
    ASSERT_TRUE(region);

    OfflineRegionMetadata newmetadata {{ 4, 5, 6 }};
    db.updateMetadata(region->getID(), newmetadata);
    auto regions = db.listRegions().value();
    EXPECT_EQ(regions.at(0).getMetadata(), newmetadata);

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, ListRegions) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    OfflineTilePyramidRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0, false };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};

    auto region = db.createRegion(definition, metadata);
    ASSERT_TRUE(region);
    auto regions = db.listRegions().value();

    ASSERT_EQ(1u, regions.size());

    EXPECT_EQ(region->getID(), regions.at(0).getID());
    regions.at(0).getDefinition().match(
            [&](OfflineTilePyramidRegionDefinition& def) {
                EXPECT_EQ(definition.styleURL, def.styleURL);
                EXPECT_EQ(definition.bounds, def.bounds);
                EXPECT_EQ(definition.minZoom, def.minZoom);
                EXPECT_EQ(definition.maxZoom, def.maxZoom);
                EXPECT_EQ(definition.pixelRatio, def.pixelRatio);
                EXPECT_EQ(definition.includeIdeographs, def.includeIdeographs);
            },
            [&](auto&) {
                EXPECT_FALSE(false);
            });
    EXPECT_EQ(metadata, regions.at(0).getMetadata());

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, GetRegionDefinition) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    OfflineTilePyramidRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0, false };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};

    EXPECT_EQ(0u, log.uncheckedCount());

    auto region = db.createRegion(definition, metadata);
    db.getRegionDefinition(region->getID())->match(
            [&](OfflineTilePyramidRegionDefinition& result) {
                EXPECT_EQ(definition.styleURL, result.styleURL);
                EXPECT_EQ(definition.bounds, result.bounds);
                EXPECT_EQ(definition.minZoom, result.minZoom);
                EXPECT_EQ(definition.maxZoom, result.maxZoom);
                EXPECT_EQ(definition.pixelRatio, result.pixelRatio);
                EXPECT_EQ(definition.includeIdeographs, result.includeIdeographs);
            },
            [&](auto&) {
                EXPECT_FALSE(false);
            }
    );
}

// Disabled due to flakiness: https://github.com/mapbox/mapbox-gl-native/issues/14966
TEST(OfflineDatabase, TEST_REQUIRES_WRITE(DISABLED_MaximumAmbientCacheSize)) {
    FixtureLog log;
    deleteDatabaseFiles();

    auto databaseSize = [] {
        return util::read_file(filename).size();
    };

    {
        OfflineDatabase db(filename);
    }

    size_t initialSize = util::read_file(filename).size();
    size_t maximumSize = 50 * 1024 * 1024;

    Response response;
    response.data = randomString(100 * 1024);

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(maximumSize); // 50 MB

        OfflineTilePyramidRegionDefinition definition{ "mapbox://style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0, true };
        OfflineRegionMetadata metadata{{ 1, 2, 3 }};

        auto region = db.createRegion(definition, metadata);

        // Add 100 MB of resources (50/50 ambient/region)
        for (unsigned i = 0; i < 250; ++i) {
            const Resource ambientTile = Resource::tile("mapbox://ambient_tile_" + std::to_string(i), 1, 0, 0, 0, Tileset::Scheme::XYZ);
            db.put(ambientTile, response);

            const Resource ambientStyle = Resource::style("mapbox://ambient_style_" + std::to_string(i));
            db.put(ambientStyle, response);

            const Resource regionTile = Resource::tile("mapbox://region_tile_" + std::to_string(i), 1, 0, 0, 0, Tileset::Scheme::XYZ);
            db.putRegionResource(region->getID(), regionTile, response);

            const Resource regionStyle = Resource::style("mapbox://region_style_" + std::to_string(i));
            db.putRegionResource(region->getID(), regionStyle, response);
        }
    }

    // We are adding about 50 MB of "region" data and 50 MB,
    // of "ambient" data. The effective size of the ambient
    // cache will be zero here because it will try to make
    // room for the region data.
    EXPECT_GE(databaseSize(), maximumSize);
    EXPECT_LE(databaseSize(), 60 * 1024 * 1024);

    maximumSize = 30 * 1024 * 1024;

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(maximumSize); // 30 MB
    }

    // Setting a new size to the ambient cache should have no
    // effect because it is all taken by offline region anyway.
    EXPECT_GE(databaseSize(), maximumSize);
    EXPECT_LE(databaseSize(), 60 * 1024 * 1024);

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(maximumSize); // 30 MB
        db.deleteRegion(std::move(db.listRegions().value()[0]));
    }

    // After deleting the offline region, the data will migrate
    // to the ambient cache, respecting the size defined.
    EXPECT_LE(databaseSize(), maximumSize);
    EXPECT_GE(databaseSize(), maximumSize / 2);

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(maximumSize * 2); // 60 MB
    }

    // Doubling the size should have no effect if
    // we don't and new tiles and if the ambient cache
    // is already under the maximum size.
    EXPECT_LE(databaseSize(), maximumSize);
    EXPECT_GE(databaseSize(), maximumSize / 2);

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(maximumSize); // 30 MB

        // Add ~50 MB in ambient cache data.
        for (unsigned i = 0; i < 250; ++i) {
            const Resource ambientTile = Resource::tile("mapbox://ambient_tile_" + std::to_string(i), 1, 0, 0, 0, Tileset::Scheme::XYZ);
            db.put(ambientTile, response);

            const Resource ambientStyle = Resource::style("mapbox://ambient_style_" + std::to_string(i));
            db.put(ambientStyle, response);
        }
    }

    // Only ambient cache now, so it should respect
    // the established size.
    EXPECT_LE(databaseSize(), maximumSize);
    EXPECT_GE(databaseSize(), maximumSize / 2);

    maximumSize = 20 * 1024 * 1024;

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(maximumSize); // 20 MB
    }

    // Should shrink again.
    EXPECT_LE(databaseSize(), maximumSize);
    EXPECT_GE(databaseSize(), initialSize);

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(0);

        for (unsigned i = 0; i < 5; ++i) {
            const Resource ambientTile = Resource::tile("mapbox://ambient_tile_" + std::to_string(i), 1, 0, 0, 0, Tileset::Scheme::XYZ);
            db.put(ambientTile, response);
            ASSERT_FALSE(db.get(ambientTile));

            const Resource ambientStyle = Resource::style("mapbox://ambient_style_" + std::to_string(i));
            db.put(ambientStyle, response);
            ASSERT_FALSE(db.get(ambientStyle));
        }
    }

    // Setting the size to zero should effectively
    // clear the cache now.
    EXPECT_EQ(initialSize, util::read_file(filename).size());
}

namespace {
std::list<std::tuple<Resource, Response>> generateResources(const std::string& tilePrefix,
                                                            const std::string& stylePrefix) {
    static const auto responseData = randomString(.5 * 1024 * 1024);
    Response response;
    response.data = responseData;
    std::list<std::tuple<Resource, Response>> resources;
    for (unsigned i = 0; i < 50; ++i) {
        resources.emplace_back(Resource::tile(tilePrefix + std::to_string(i), 1, 0, 0, 0, Tileset::Scheme::XYZ),
                               response);
        resources.emplace_back(Resource::style(stylePrefix + std::to_string(i)), response);
    }
    return resources;
}
} // namespace

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(DeleteRegion)) {
    FixtureLog log;
    deleteDatabaseFiles();

    {
        OfflineDatabase dbCreate(filename);
    }

    size_t initialSize = util::read_file(filename).size();

    {
        Response response;
        response.data = randomString(.5 * 1024 * 1024);

        OfflineDatabase db(filename);

        OfflineTilePyramidRegionDefinition definition{ "mapbox://style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0, true };
        OfflineRegionMetadata metadata{{ 1, 2, 3 }};

        auto region1 = db.createRegion(definition, metadata);
        auto region2 = db.createRegion(definition, metadata);

        OfflineRegionStatus status;
        db.putRegionResources(region1->getID(), generateResources("mapbox://tile_1", "mapbox://style_1"), status);
        db.putRegionResources(region2->getID(), generateResources("mapbox://tile_2", "mapbox://style_2"), status);
        const size_t sizeWithTwoRegions = util::read_file(filename).size();

        db.runPackDatabaseAutomatically(false);
        db.deleteRegion(std::move(*region1));

        ASSERT_EQ(1u, db.listRegions().value().size());
        // Region is removed but the size of the database is the same.
        EXPECT_EQ(sizeWithTwoRegions, util::read_file(filename).size());

        db.pack();
        // The size of the database has shrunk after pack().
        const size_t sizeWithOneRegion = util::read_file(filename).size();
        EXPECT_LT(sizeWithOneRegion, sizeWithTwoRegions);
        db.runPackDatabaseAutomatically(true);
        db.deleteRegion(std::move(*region2));
        // The size of the database has shrunk right away.
        const size_t sizeWithoutRegions = util::read_file(filename).size();
        ASSERT_EQ(0u, db.listRegions().value().size());
        EXPECT_LT(sizeWithoutRegions, sizeWithOneRegion);

        // The tiles from the offline region will migrate to the
        // ambient cache and shrink the database to the maximum
        // size defined by default.
        EXPECT_LE(sizeWithoutRegions, util::DEFAULT_MAX_CACHE_SIZE);

        // After clearing the cache, the size of the database
        // should get back to the original size.
        db.clearAmbientCache();
    }

    EXPECT_EQ(initialSize, util::read_file(filename).size());
    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(Pack)) {
    FixtureLog log;
    deleteDatabaseFiles();

    OfflineDatabase db(filename);
    size_t initialSize = util::read_file(filename).size();
    db.runPackDatabaseAutomatically(false);

    Response response;
    response.data = randomString(.5 * 1024 * 1024);

    for (unsigned i = 0; i < 50; ++i) {
        const Resource tile = Resource::tile("mapbox://tile_" + std::to_string(i), 1, 0, 0, 0, Tileset::Scheme::XYZ);
        db.put(tile, response);

        const Resource style = Resource::style("mapbox://style_" + std::to_string(i));
        db.put(style, response);
    }
    size_t populatedSize = util::read_file(filename).size();
    ASSERT_GT(populatedSize, initialSize);

    db.clearAmbientCache();
    EXPECT_EQ(populatedSize, util::read_file(filename).size());
    EXPECT_EQ(0u, log.uncheckedCount());

    db.pack();
    EXPECT_EQ(initialSize, util::read_file(filename).size());
    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, MapboxTileLimitExceeded) {
    FixtureLog log;

    uint64_t limit = 60;

    OfflineDatabase db(":memory:");
    db.setOfflineMapboxTileCountLimit(limit);

    Response response;
    response.data = randomString(4096);

    auto insertAmbientTile = [&](unsigned i) {
        const Resource ambientTile = Resource::tile("mapbox://ambient_tile_" + std::to_string(i), 1, 0, 0, 0, Tileset::Scheme::XYZ);
        db.put(ambientTile, response);
    };

    auto insertRegionTile = [&](int64_t regionID, unsigned i) {
        const Resource tile = Resource::tile("mapbox://region_tile_" + std::to_string(i), 1, 0, 0, 0, Tileset::Scheme::XYZ);
        db.putRegionResource(regionID, tile, response);
    };

    OfflineTilePyramidRegionDefinition definition1{ "mapbox://style1", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0, true };
    OfflineRegionMetadata metadata1{{ 1, 2, 3 }};

    OfflineTilePyramidRegionDefinition definition2{ "mapbox://style2", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0, true };
    OfflineRegionMetadata metadata2{{ 1, 2, 3 }};

    auto region1 = db.createRegion(definition1, metadata1);
    auto region2 = db.createRegion(definition2, metadata2);

    // Fine because tile limit only affects offline region.
    for (unsigned i = 0; i < limit * 2; ++i) {
        insertAmbientTile(i);
    }

    ASSERT_EQ(db.getOfflineMapboxTileCount(), 0);

    // Fine because this region is under the tile limit.
    for (unsigned i = 0; i < limit - 10; ++i) {
        insertRegionTile(region1->getID(), i);
    }

    ASSERT_EQ(db.getOfflineMapboxTileCount(), limit - 10);

    // Fine because this region + the previous is at the limit.
    for (unsigned i = limit; i < limit + 10; ++i) {
        insertRegionTile(region2->getID(), i);
    }

    ASSERT_EQ(db.getOfflineMapboxTileCount(), limit);

    // Full.
    ASSERT_THROW(insertRegionTile(region1->getID(), 200), MapboxTileLimitExceededException);
    ASSERT_THROW(insertRegionTile(region2->getID(), 201), MapboxTileLimitExceededException);

    // These tiles are already on respective
    // regions.
    insertRegionTile(region1->getID(), 0);
    insertRegionTile(region2->getID(), 60);

    // Should be fine, ambient tile.
    insertAmbientTile(333);

    // Also fine, not Mapbox.
    const Resource notMapboxTile = Resource::tile("foobar://region_tile", 1, 0, 0, 0, Tileset::Scheme::XYZ);
    db.putRegionResource(region1->getID(), notMapboxTile, response);

    // These tiles are not on the region they are
    // being added to, but exist on another region,
    // so they do not add to the total size.
    insertRegionTile(region2->getID(), 0);
    insertRegionTile(region1->getID(), 60);

    ASSERT_EQ(db.getOfflineMapboxTileCount(), limit);

    // The tile 1 belongs to two regions and will
    // still count as resource.
    db.deleteRegion(std::move(*region2));

    ASSERT_EQ(db.getOfflineMapboxTileCount(), 51);

    // Add new tiles to the region 1. We are adding
    // 10, which would blow up the limit if it wasn't
    // for the fact that tile 60 is already on the
    // database and will not count.
    for (unsigned i = limit; i < limit + 10; ++i) {
        insertRegionTile(region1->getID(), i);
    }

    // Full again.
    ASSERT_THROW(insertRegionTile(region1->getID(), 202), MapboxTileLimitExceededException);

    db.deleteRegion(std::move(*region1));

    ASSERT_EQ(0u, db.listRegions().value().size());
    ASSERT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, Invalidate) {
    using namespace std::chrono_literals;

    FixtureLog log;
    OfflineDatabase db(":memory:");

    Response response;
    response.noContent = true;
    response.mustRevalidate = false;
    response.expires = util::now() + 1h;

    const Resource ambientTile = Resource::tile("mapbox://tile_ambient", 1, 0, 0, 0, Tileset::Scheme::XYZ);
    db.put(ambientTile, response);

    const Resource ambientStyle  = Resource::style("mapbox://style_ambient");
    db.put(ambientStyle, response);

    OfflineTilePyramidRegionDefinition definition { "mapbox://style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0, true };
    OfflineRegionMetadata metadata {{ 1, 2, 3 }};

    auto region1 = db.createRegion(definition, metadata);
    const Resource region1Tile = Resource::tile("mapbox://tile_offline_region1", 1.0, 0, 0, 0, Tileset::Scheme::XYZ);
    db.putRegionResource(region1->getID(), region1Tile, response);

    const Resource region1Style = Resource::style("mapbox://style_offline_region1");
    db.putRegionResource(region1->getID(), region1Style, response);

    auto region2 = db.createRegion(definition, metadata);
    const Resource region2Tile = Resource::tile("mapbox://tile_offline_region2", 1.0, 0, 0, 0, Tileset::Scheme::XYZ);
    db.putRegionResource(region2->getID(), region2Tile, response);

    const Resource region2Style = Resource::style("mapbox://style_offline_region2");
    db.putRegionResource(region2->getID(), region2Style, response);

    // Prior to invalidation, all tiles are usable.
    EXPECT_TRUE(db.get(ambientTile)->isUsable());
    EXPECT_TRUE(db.get(ambientStyle)->isUsable());
    EXPECT_TRUE(db.get(region1Tile)->isUsable());
    EXPECT_TRUE(db.get(region1Style)->isUsable());
    EXPECT_TRUE(db.get(region2Tile)->isUsable());
    EXPECT_TRUE(db.get(region2Style)->isUsable());

    // Invalidate a region will not invalidate ambient
    // tiles or other regions.
    EXPECT_TRUE(db.invalidateRegion(region1->getID()) == nullptr);

    EXPECT_TRUE(db.get(ambientTile)->isUsable());
    EXPECT_TRUE(db.get(ambientStyle)->isUsable());
    EXPECT_FALSE(db.get(region1Tile)->isUsable());
    EXPECT_FALSE(db.get(region1Style)->isUsable());
    EXPECT_TRUE(db.get(region2Tile)->isUsable());
    EXPECT_TRUE(db.get(region2Style)->isUsable());

    // Invalidate the ambient cache will not invalidate
    // the regions that are still valid.
    EXPECT_TRUE(db.invalidateAmbientCache() == nullptr);

    EXPECT_FALSE(db.get(ambientTile)->isUsable());
    EXPECT_FALSE(db.get(ambientStyle)->isUsable());
    EXPECT_FALSE(db.get(region1Tile)->isUsable());
    EXPECT_FALSE(db.get(region1Style)->isUsable());
    EXPECT_TRUE(db.get(region2Tile)->isUsable());
    EXPECT_TRUE(db.get(region2Style)->isUsable());

    // Sanity check.
    EXPECT_TRUE(db.get(ambientTile)->expires < util::now());
    EXPECT_TRUE(db.get(ambientStyle)->expires < util::now());
    EXPECT_TRUE(db.get(region1Tile)->expires < util::now());
    EXPECT_TRUE(db.get(region1Style)->expires < util::now());
    EXPECT_TRUE(db.get(region2Tile)->expires > util::now());
    EXPECT_TRUE(db.get(region2Style)->expires > util::now());

    EXPECT_TRUE(db.get(ambientTile)->mustRevalidate);
    EXPECT_TRUE(db.get(ambientStyle)->mustRevalidate);
    EXPECT_TRUE(db.get(region1Tile)->mustRevalidate);
    EXPECT_TRUE(db.get(region1Style)->mustRevalidate);
    EXPECT_FALSE(db.get(region2Tile)->mustRevalidate);
    EXPECT_FALSE(db.get(region2Style)->mustRevalidate);

    // Should not throw.
    EXPECT_TRUE(db.invalidateRegion(region2->getID()) == nullptr);
    EXPECT_TRUE(db.invalidateRegion(region2->getID()) == nullptr);
    EXPECT_TRUE(db.invalidateRegion(123) == nullptr);

    // Invalidate != delete.
    auto regions = db.listRegions().value();
    ASSERT_EQ(2u, regions.size());

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(ClearAmbientCache)) {
    FixtureLog log;
    deleteDatabaseFiles();

    {
        OfflineDatabase dbCreate(filename);
    }

    size_t initialSize = util::read_file(filename).size();

    {
        Response response;
        response.data = randomString(.5 * 1024 * 1024);

        OfflineDatabase db(filename);

        for (unsigned i = 0; i < 50; ++i) {
            const Resource tile = Resource::tile("mapbox://tile_" + std::to_string(i), 1, 0, 0, 0, Tileset::Scheme::XYZ);
            db.put(tile, response);

            const Resource style = Resource::style("mapbox://style_" + std::to_string(i));
            db.put(style, response);
        }

        db.clearAmbientCache();
    }

    EXPECT_EQ(initialSize, util::read_file(filename).size());
    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, CreateRegionInfiniteMaxZoom) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    OfflineTilePyramidRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0, false };
    OfflineRegionMetadata metadata;
    auto region = db.createRegion(definition, metadata);
    ASSERT_TRUE(region);

    EXPECT_EQ(0u, log.uncheckedCount());

    region->getDefinition().match([&](auto& def) {
        EXPECT_EQ(0, def.minZoom);
        EXPECT_EQ(INFINITY, def.maxZoom);
    });
}

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(ConcurrentUse)) {
    FixtureLog log;
    deleteDatabaseFiles();

    OfflineDatabase db1(filename);
    EXPECT_EQ(0u, log.uncheckedCount());

    OfflineDatabase db2(filename);

    std::thread thread1([&] {
        for (auto i = 0; i < 100; i++) {
            db1.put(fixture::resource, fixture::response);
            EXPECT_TRUE(bool(db1.get(fixture::resource)));
        }
    });

    std::thread thread2([&] {
        for (auto i = 0; i < 100; i++) {
            db2.put(fixture::resource, fixture::response);
            EXPECT_TRUE(bool(db2.get(fixture::resource)));

            if (i == 50) {
                db2.clearAmbientCache();
            }
        }
    });

    thread1.join();
    thread2.join();

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, PutReturnsSize) {
    FixtureLog log;
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

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, PutEvictsLeastRecentlyUsedResources) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    db.setMaximumAmbientCacheSize(1024 * 100);

    Response response;
    response.data = randomString(1024);

    for (uint32_t i = 1; i <= 100; i++) {
        Resource resource = Resource::style("http://example.com/"s + util::toString(i));
        db.put(resource, response);
        EXPECT_TRUE(bool(db.get(resource))) << i;
    }

    EXPECT_FALSE(bool(db.get(Resource::style("http://example.com/1"))));

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, PutRegionResourceDoesNotEvict) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    db.setMaximumAmbientCacheSize(1024 * 100);

    OfflineTilePyramidRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0, true };
    auto region = db.createRegion(definition, OfflineRegionMetadata());
    ASSERT_TRUE(region);

    Response response;
    response.data = randomString(1024);

    for (uint32_t i = 1; i <= 100; i++) {
        db.putRegionResource(region->getID(), Resource::style("http://example.com/"s + util::toString(i)), response);
    }

    EXPECT_TRUE(bool(db.get(Resource::style("http://example.com/1"))));
    EXPECT_TRUE(bool(db.get(Resource::style("http://example.com/20"))));

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, PutFailsWhenEvictionInsuffices) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    db.setMaximumAmbientCacheSize(1024 * 100);

    Response big;
    big.data = randomString(1024 * 100);

    EXPECT_FALSE(db.put(Resource::style("http://example.com/big"), big).first);

    EXPECT_EQ(1u, log.count({ EventSeverity::Info, Event::Database, -1, "Unable to make space for entry" }));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_FALSE(bool(db.get(Resource::style("http://example.com/big"))));

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, GetRegionCompletedStatus) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    OfflineTilePyramidRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0, false };
    OfflineRegionMetadata metadata;
    auto region = db.createRegion(definition, metadata);
    ASSERT_TRUE(region);

    auto status1 = db.getRegionCompletedStatus(region->getID());
    ASSERT_TRUE(status1);
    EXPECT_EQ(0u, status1->completedResourceCount);
    EXPECT_EQ(0u, status1->completedResourceSize);
    EXPECT_EQ(0u, status1->completedTileCount);
    EXPECT_EQ(0u, status1->completedTileSize);

    Response response;
    response.data = std::make_shared<std::string>("data");

    uint64_t styleSize = db.putRegionResource(region->getID(), Resource::style("http://example.com/"), response);

    auto status2 = db.getRegionCompletedStatus(region->getID());
    ASSERT_TRUE(status2);
    EXPECT_EQ(1u, status2->completedResourceCount);
    EXPECT_EQ(styleSize, status2->completedResourceSize);
    EXPECT_EQ(0u, status2->completedTileCount);
    EXPECT_EQ(0u, status2->completedTileSize);

    uint64_t tileSize = db.putRegionResource(region->getID(), Resource::tile("http://example.com/", 1.0, 0, 0, 0, Tileset::Scheme::XYZ), response);

    auto status3 = db.getRegionCompletedStatus(region->getID());
    ASSERT_TRUE(status3);
    EXPECT_EQ(2u, status3->completedResourceCount);
    EXPECT_EQ(styleSize + tileSize, status3->completedResourceSize);
    EXPECT_EQ(1u, status3->completedTileCount);
    EXPECT_EQ(tileSize, status3->completedTileSize);

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, HasRegionResource) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    db.setMaximumAmbientCacheSize(1024 * 100);

    OfflineTilePyramidRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0, true };
    auto region = db.createRegion(definition, OfflineRegionMetadata());
    ASSERT_TRUE(region);

    EXPECT_FALSE(bool(db.hasRegionResource(Resource::style("http://example.com/1"))));
    EXPECT_FALSE(bool(db.hasRegionResource(Resource::style("http://example.com/20"))));

    Response response;
    response.data = randomString(1024);

    for (uint32_t i = 1; i <= 100; i++) {
        db.putRegionResource(region->getID(), Resource::style("http://example.com/"s + util::toString(i)), response);
    }

    EXPECT_TRUE(bool(db.hasRegionResource(Resource::style("http://example.com/1"))));
    EXPECT_TRUE(bool(db.hasRegionResource(Resource::style("http://example.com/20"))));
    EXPECT_EQ(1024, *(db.hasRegionResource(Resource::style("http://example.com/20"))));

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, HasRegionResourceTile) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    db.setMaximumAmbientCacheSize(1024 * 100);

    OfflineTilePyramidRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0, false };
    auto region = db.createRegion(definition, OfflineRegionMetadata());
    ASSERT_TRUE(region);

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

    EXPECT_FALSE(bool(db.hasRegionResource(resource)));
    db.putRegionResource(region->getID(), resource, response);
    EXPECT_TRUE(bool(db.hasRegionResource(resource)));
    EXPECT_EQ(5, *(db.hasRegionResource(resource)));

    auto anotherRegion = db.createRegion(definition, OfflineRegionMetadata());
    ASSERT_TRUE(anotherRegion);
    EXPECT_LT(region->getID(), anotherRegion->getID());
    EXPECT_TRUE(bool(db.hasRegionResource(resource)));
    EXPECT_EQ(5, *(db.hasRegionResource(resource)));

    EXPECT_EQ(0u, log.uncheckedCount());

}

TEST(OfflineDatabase, OfflineMapboxTileCount) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    OfflineTilePyramidRegionDefinition definition { "http://example.com/style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0 , true};
    OfflineRegionMetadata metadata;

    auto region1 = db.createRegion(definition, metadata);
    ASSERT_TRUE(region1);
    auto region2 = db.createRegion(definition, metadata);
    ASSERT_TRUE(region2);

    Resource nonMapboxTile = Resource::tile("http://example.com/", 1.0, 0, 0, 0, Tileset::Scheme::XYZ);
    Resource mapboxTile1 = Resource::tile("mapbox://tiles/1", 1.0, 0, 0, 0, Tileset::Scheme::XYZ);
    Resource mapboxTile2 = Resource::tile("mapbox://tiles/2", 1.0, 0, 0, 1, Tileset::Scheme::XYZ);

    Response response;
    response.data = std::make_shared<std::string>("data");

    // Count is initially zero.
    EXPECT_EQ(0u, db.getOfflineMapboxTileCount());

    // Count stays the same after putting a non-tile resource.
    db.putRegionResource(region1->getID(), Resource::style("http://example.com/"), response);
    EXPECT_EQ(0u, db.getOfflineMapboxTileCount());

    // Count stays the same after putting a non-Mapbox tile.
    db.putRegionResource(region1->getID(), nonMapboxTile, response);
    EXPECT_EQ(0u, db.getOfflineMapboxTileCount());

    // Count increases after putting a Mapbox tile not used by another region.
    db.putRegionResource(region1->getID(), mapboxTile1, response);
    EXPECT_EQ(1u, db.getOfflineMapboxTileCount());

    // Count stays the same after putting a Mapbox tile used by another region.
    db.putRegionResource(region2->getID(), mapboxTile1, response);
    EXPECT_EQ(1u, db.getOfflineMapboxTileCount());

    // Count stays the same after putting a Mapbox tile used by the same region.
    db.putRegionResource(region2->getID(), mapboxTile1, response);
    EXPECT_EQ(1u, db.getOfflineMapboxTileCount());

    // Count stays the same after deleting a region when the tile is still used by another region.
    db.deleteRegion(std::move(*region2));
    EXPECT_EQ(1u, db.getOfflineMapboxTileCount());

    // Count stays the same after the putting a non-offline Mapbox tile.
    db.put(mapboxTile2, response);
    EXPECT_EQ(1u, db.getOfflineMapboxTileCount());

    // Count increases after putting a pre-existing, but non-offline Mapbox tile.
    db.putRegionResource(region1->getID(), mapboxTile2, response);
    EXPECT_EQ(2u, db.getOfflineMapboxTileCount());

    // Count decreases after deleting a region when the tiles are not used by other regions.
    db.deleteRegion(std::move(*region1));
    EXPECT_EQ(0u, db.getOfflineMapboxTileCount());

    EXPECT_EQ(0u, log.uncheckedCount());
}


TEST(OfflineDatabase, BatchInsertion) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    db.setMaximumAmbientCacheSize(1024 * 100);

    OfflineTilePyramidRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0, true };
    auto region = db.createRegion(definition, OfflineRegionMetadata());
    ASSERT_TRUE(region);

    Response response;
    response.data = randomString(1024);
    std::list<std::tuple<Resource, Response>> resources;

    for (uint32_t i = 1; i <= 100; i++) {
        resources.emplace_back(Resource::style("http://example.com/"s + util::toString(i)), response);
    }

    OfflineRegionStatus status;
    db.putRegionResources(region->getID(), resources, status);

    for (uint32_t i = 1; i <= 100; i++) {
        EXPECT_TRUE(bool(db.get(Resource::style("http://example.com/"s + util::toString(i)))));
    }

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, BatchInsertionMapboxTileCountExceeded) {
    FixtureLog log;
    OfflineDatabase db(":memory:");
    db.setOfflineMapboxTileCountLimit(1);
    db.setMaximumAmbientCacheSize(1024 * 100);

    OfflineTilePyramidRegionDefinition definition { "", LatLngBounds::world(), 0, INFINITY, 1.0, false };
    auto region = db.createRegion(definition, OfflineRegionMetadata());
    ASSERT_TRUE(region);

    Response response;
    response.data = randomString(1024);
    std::list<std::tuple<Resource, Response>> resources;

    resources.emplace_back(Resource::style("http://example.com/"), response);
    resources.emplace_back(Resource::tile("mapbox://tiles/1", 1.0, 0, 0, 0, Tileset::Scheme::XYZ), response);
    resources.emplace_back(Resource::tile("mapbox://tiles/2", 1.0, 0, 0, 0, Tileset::Scheme::XYZ), response);

    OfflineRegionStatus status;
    try {
        db.putRegionResources(region->getID(), resources, status);
        EXPECT_FALSE(true);
    } catch (const MapboxTileLimitExceededException&) {
        // Expected
    }

    EXPECT_EQ(0u, status.completedTileCount);
    EXPECT_EQ(0u, status.completedResourceCount);
    const auto completedStatus = db.getRegionCompletedStatus(region->getID()).value();
    EXPECT_EQ(1u, completedStatus.completedTileCount);
    EXPECT_EQ(2u, completedStatus.completedResourceCount);

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, MigrateFromV2Schema) {
    // v2.db is a v2 database containing a single offline region with a small number of resources.
    FixtureLog log;
    deleteDatabaseFiles();
    util::copyFile(filename, "test/fixtures/offline_database/v2.db");

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(0);

        auto regions = db.listRegions();
        ASSERT_TRUE(regions);
        for (auto& region : regions.value()) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion(filename));
    EXPECT_LT(databasePageCount(filename),
              databasePageCount("test/fixtures/offline_database/v2.db"));

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, MigrateFromV3Schema) {
    // v3.db is a v3 database, migrated from v2.
    FixtureLog log;
    deleteDatabaseFiles();
    util::copyFile(filename, "test/fixtures/offline_database/v3.db");

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(0);

        auto regions = db.listRegions().value();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion(filename));

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, MigrateFromV4Schema) {
    // v4.db is a v4 database, migrated from v2 & v3. This database used `journal_mode = WAL` and `synchronous = NORMAL`.
    FixtureLog log;
    deleteDatabaseFiles();
    util::copyFile(filename, "test/fixtures/offline_database/v4.db");

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(0);

        auto regions = db.listRegions().value();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion(filename));

    // Journal mode should be DELETE after migration to v5.
    EXPECT_EQ("delete", databaseJournalMode(filename));

    // Synchronous setting should be FULL (2) after migration to v5.
    EXPECT_EQ(2, databaseSyncMode(filename));

    EXPECT_EQ(0u, log.uncheckedCount());
}


TEST(OfflineDatabase, MigrateFromV5Schema) {
    // v5.db is a v5 database, migrated from v2, v3 & v4.
    FixtureLog log;
    deleteDatabaseFiles();
    util::copyFile(filename, "test/fixtures/offline_database/v5.db");

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(0);

        auto regions = db.listRegions().value();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(6, databaseUserVersion(filename));

    EXPECT_EQ((std::vector<std::string>{"id",
                                        "url_template",
                                        "pixel_ratio",
                                        "z",
                                        "x",
                                        "y",
                                        "expires",
                                        "modified",
                                        "etag",
                                        "data",
                                        "compressed",
                                        "accessed",
                                        "must_revalidate"}),
              databaseTableColumns(filename, "tiles"));
    EXPECT_EQ(
        (std::vector<std::string>{
            "id", "url", "kind", "expires", "modified", "etag", "data", "compressed", "accessed", "must_revalidate"}),
        databaseTableColumns(filename, "resources"));

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, IncrementalVacuum) {
    FixtureLog log;
    deleteDatabaseFiles();
    util::copyFile(filename, "test/fixtures/offline_database/no_auto_vacuum.db");
    EXPECT_EQ(0, databaseAutoVacuum(filename));

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(0);

        auto regions = db.listRegions().value();
        for (auto& region : regions) {
            db.deleteRegion(std::move(region));
        }
    }

    EXPECT_EQ(2, databaseAutoVacuum(filename));

    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, DowngradeSchema) {
    // v999.db is a v999 database, it should be deleted
    // and recreated with the current schema.
    FixtureLog log;
    util::deleteFile(filename);
    util::copyFile(filename, "test/fixtures/offline_database/v999.db");

    {
        OfflineDatabase db(filename);
        db.setMaximumAmbientCacheSize(0);
    }

    EXPECT_EQ(6, databaseUserVersion(filename));

    EXPECT_EQ((std::vector<std::string>{ "id", "url_template", "pixel_ratio", "z", "x", "y",
                                         "expires", "modified", "etag", "data", "compressed",
                                         "accessed", "must_revalidate" }),
              databaseTableColumns(filename, "tiles"));
    EXPECT_EQ((std::vector<std::string>{ "id", "url", "kind", "expires", "modified", "etag", "data",
                                         "compressed", "accessed", "must_revalidate" }),
              databaseTableColumns(filename, "resources"));

    EXPECT_EQ(1u, log.count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));
    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, CorruptDatabaseOnOpen) {
    FixtureLog log;
    util::deleteFile(filename);
    util::copyFile(filename, "test/fixtures/offline_database/corrupt-immediate.db");

    // This database is corrupt in a way that will prevent opening the database.
    OfflineDatabase db(filename);
    EXPECT_EQ(1u, log.count(error(ResultCode::Corrupt, "Can't open database: database disk image is malformed"), true));
    EXPECT_EQ(1u, log.count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));
    EXPECT_EQ(0u, log.uncheckedCount());

    // Now try inserting and reading back to make sure we have a valid database.
    for (const auto& res : { fixture::resource, fixture::tile }) {
        EXPECT_EQ(std::make_pair(true, uint64_t(5)), db.put(res, fixture::response));
        EXPECT_EQ(0u, log.uncheckedCount());
        auto result = db.get(res);
        EXPECT_EQ(0u, log.uncheckedCount());
        ASSERT_TRUE(result && result->data);
        EXPECT_EQ("first", *result->data);
    }
}

TEST(OfflineDatabase, CorruptDatabaseOnQuery) {
    FixtureLog log;
    util::deleteFile(filename);
    util::copyFile(filename, "test/fixtures/offline_database/corrupt-delayed.db");

    // This database is corrupt in a way that won't manifest itself until we start querying it,
    // so just opening it will not cause an error.
    OfflineDatabase db(filename);

    // Just opening this corrupt database should not have produced an error yet, since
    // PRAGMA user_version still succeeds with this database.
    EXPECT_EQ(0u, log.uncheckedCount());

    // The first request fails because the database is corrupt and has to be recreated.
    EXPECT_EQ(nullopt, db.get(fixture::tile));
    EXPECT_EQ(1u, log.count(error(ResultCode::Corrupt, "Can't read resource: database disk image is malformed"), true));
    EXPECT_EQ(1u, log.count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));
    EXPECT_EQ(0u, log.uncheckedCount());

    // Now try inserting and reading back to make sure we have a valid database.
    for (const auto& res : { fixture::resource, fixture::tile }) {
        EXPECT_EQ(std::make_pair(true, uint64_t(5)), db.put(res, fixture::response));
        EXPECT_EQ(0u, log.uncheckedCount());
        auto result = db.get(res);
        EXPECT_EQ(0u, log.uncheckedCount());
        ASSERT_TRUE(result && result->data);
        EXPECT_EQ("first", *result->data);
    }
}

#ifndef __QT__ // Qt doesn't expose the ability to register virtual file system handlers.
TEST(OfflineDatabase, TEST_REQUIRES_WRITE(DisallowedIO)) {
    FixtureLog log;
    deleteDatabaseFiles();
    test::SQLite3TestFS fs;

    OfflineDatabase db(filename_test_fs);
    EXPECT_EQ(0u, log.uncheckedCount());

    // First, create a region object so that we can try deleting it later.
    OfflineTilePyramidRegionDefinition definition(
        "mapbox://style", LatLngBounds::hull({ 37.66, -122.57 }, { 37.83, -122.32 }), 0, 8, 2, false);
    auto region = db.createRegion(definition, {});
    ASSERT_TRUE(region);

    // Now forbid any type of IO on the database and test that none of the calls crashes.
    fs.allowIO(false);

    EXPECT_EQ(nullopt, db.get(fixture::resource));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't update timestamp: authorization denied")));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't read resource: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_EQ(std::make_pair(false, uint64_t(0)), db.put(fixture::resource, fixture::response));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't write resource: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_FALSE(db.listRegions());
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't list regions: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_FALSE(db.createRegion(definition, {}));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't create region: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_FALSE(db.updateMetadata(region->getID(), {}));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't update region metadata: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_EQ(nullopt, db.getRegionResource(fixture::resource));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't update timestamp: authorization denied")));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't read region resource: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_EQ(nullopt, db.hasRegionResource(fixture::resource));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't query region resource: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_EQ(0u, db.putRegionResource(region->getID(), fixture::resource, fixture::response));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't write region resource: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    OfflineRegionStatus status;
    db.putRegionResources(region->getID(), { std::make_tuple(fixture::resource, fixture::response) }, status);
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't write region resources: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_FALSE(db.getRegionDefinition(region->getID()));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't load region: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_FALSE(db.getRegionCompletedStatus(region->getID()));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't get region status: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_TRUE(db.deleteRegion(std::move(*region)));
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't delete region: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    EXPECT_EQ(std::numeric_limits<uint64_t>::max(), db.getOfflineMapboxTileCount());
    EXPECT_EQ(1u, log.count(warning(ResultCode::Auth, "Can't get offline Mapbox tile count: authorization denied")));
    EXPECT_EQ(0u, log.uncheckedCount());

    fs.reset();
}
#endif // __QT__

TEST(OfflineDatabase, MergeDatabaseWithSingleRegion_New) {
    util::deleteFile(filename_sideload);
    util::copyFile(filename_sideload, "test/fixtures/offline_database/sideload_sat.db");

    OfflineDatabase db(":memory:");
    EXPECT_EQ(0u, db.listRegions()->size());

    auto result = db.mergeDatabase(filename_sideload);
    EXPECT_EQ(1u, result->size());
    EXPECT_EQ(1u, db.listRegions()->size());

    auto regionId = result->front().getID();
    auto status =  db.getRegionCompletedStatus(regionId);
    EXPECT_EQ(2u, status->completedResourceCount);
    EXPECT_EQ(1u, status->completedTileCount);
}

TEST(OfflineDatabase, TEST_REQUIRES_WRITE(MergeDatabaseWithSingleRegion_Update)) {
    deleteDatabaseFiles();
    util::deleteFile(filename_sideload);
    util::copyFile(filename, "test/fixtures/offline_database/satellite_test.db");
    util::copyFile(filename_sideload, "test/fixtures/offline_database/sideload_sat.db");
    int64_t regionId;

    {
        OfflineDatabase db(filename);
        auto regions = db.listRegions();
        EXPECT_EQ(1u, db.listRegions()->size());
        regionId = regions->front().getID();

        auto result = db.mergeDatabase(filename_sideload);
        EXPECT_EQ(1u, result->size());
        // When updating an identical region, the region id remains unchanged.
        EXPECT_EQ(regionId, result->front().getID());

        auto status = db.getRegionCompletedStatus(regionId);
        EXPECT_EQ(5u, status->completedResourceCount);
        EXPECT_EQ(1u, status->completedTileCount);

        //Verify the modified timestamp matches the tile in the sideloaded db.
        auto updatedTile = db.getRegionResource(
            Resource::tile("mapbox://tiles/mapbox.satellite/{z}/{x}/{y}{ratio}.webp",
                1, 0, 0, 1, Tileset::Scheme::XYZ));
        EXPECT_EQ(Timestamp{ Seconds(1520409600) }, *(updatedTile->first.modified));
    }
}

TEST(OfflineDatabase, MergeDatabaseWithSingleRegion_NoUpdate) {
    deleteDatabaseFiles();
    util::deleteFile(filename_sideload);

    //Swap sideload/main database from update test and ensure that an older tile is not copied over
    util::copyFile(filename_sideload, "test/fixtures/offline_database/satellite_test.db");
    util::copyFile(filename, "test/fixtures/offline_database/sideload_sat.db");

    OfflineDatabase db(filename);
    auto result = db.mergeDatabase(filename_sideload);
    EXPECT_EQ(1u, result->size());
    EXPECT_EQ(1u, db.listRegions()->size());

    auto updatedTile = db.getRegionResource(
        Resource::tile("mapbox://tiles/mapbox.satellite/{z}/{x}/{y}{ratio}.webp",
            1, 0, 0, 1, Tileset::Scheme::XYZ));

    //Verify the modified timestamp matches the tile in the main db.
    EXPECT_EQ(Timestamp{ Seconds(1520409600) }, *(updatedTile->first.modified));
}

TEST(OfflineDatabase, MergeDatabaseWithSingleRegion_AmbientTiles) {
    util::deleteFile(filename_sideload);
    util::copyFile(filename_sideload, "test/fixtures/offline_database/sideload_ambient.db");

    OfflineDatabase db(":memory:");
    auto result = db.mergeDatabase(filename_sideload);

    EXPECT_TRUE(bool(db.hasRegionResource(Resource::tile("mapbox://tiles/mapbox.satellite/{z}/{x}/{y}{ratio}.png", 1, 0, 0, 1, Tileset::Scheme::XYZ))));

    //Ambient resources should not be copied
    EXPECT_FALSE(bool(db.hasRegionResource(Resource::style("mapbox://styles/mapbox/streets-v9"))));
    EXPECT_FALSE(bool(db.hasRegionResource(Resource::tile("mapbox://tiles/mapbox.satellite/{z}/{x}/{y}{ratio}.png", 1, 0, 1, 2, Tileset::Scheme::XYZ))));
    EXPECT_FALSE(bool(db.hasRegionResource(Resource::tile("mapbox://tiles/mapbox.satellite/{z}/{x}/{y}{ratio}.png", 1, 1, 1, 2, Tileset::Scheme::XYZ))));
}

TEST(OfflineDatabase, MergeDatabaseWithMultipleRegions_New) {
    util::deleteFile(filename_sideload);
    util::copyFile(filename_sideload, "test/fixtures/offline_database/sideload_sat_multiple.db");

    OfflineDatabase db(":memory:");
    EXPECT_EQ(0u, db.listRegions()->size());

    auto result = db.mergeDatabase(filename_sideload);
    EXPECT_EQ(2u, result->size());
    EXPECT_EQ(2u, db.listRegions()->size());
    
    auto region1Id = result->front().getID();
    auto status = db.getRegionCompletedStatus(region1Id);
    EXPECT_EQ(4u, status->completedResourceCount);
    EXPECT_EQ(3u, status->completedTileCount);

    auto region2Id = result->back().getID();
    status = db.getRegionCompletedStatus(region2Id);
    EXPECT_EQ(1u, status->completedTileCount);
    EXPECT_EQ(2u, status->completedResourceCount);
}

TEST(OfflineDatabase, MergeDatabaseWithMultipleRegionsWithOverlap) {
    deleteDatabaseFiles();
    util::deleteFile(filename_sideload);
    util::copyFile(filename, "test/fixtures/offline_database/sideload_sat.db");
    util::copyFile(filename_sideload, "test/fixtures/offline_database/sideload_sat_multiple.db");

    {
        OfflineDatabase db(filename);
        EXPECT_EQ(1u, db.listRegions()->size());

        auto result = db.mergeDatabase(filename_sideload);
        EXPECT_EQ(2u, result->size());
        EXPECT_EQ(3u, db.listRegions()->size());
    }

    mapbox::sqlite::Database db = mapbox::sqlite::Database::open(filename, mapbox::sqlite::ReadOnly);
    {
        // clang-format off
        mapbox::sqlite::Statement stmt { db, "SELECT COUNT(*) "
                                        "FROM region_tiles "
                                        "JOIN tiles WHERE tile_id = id" };
        // clang-format on
        mapbox::sqlite::Query query { stmt };
        query.run();

        // Ensure multiple entries for tiles shared between regions
        EXPECT_EQ(1 + 3 + 1, query.get<int>(0));
    }
    {
        // clang-format off
        mapbox::sqlite::Statement stmt { db, "SELECT COUNT(*) "
                                        "FROM region_resources "
                                        "JOIN resources WHERE resource_id = id" };
        // clang-format on
        mapbox::sqlite::Query query { stmt };
        query.run();

        // Ensure multiple entries for resources shared between regions
        EXPECT_EQ(1 + 1 + 1, query.get<int>(0));
    }
}

TEST(OfflineDatabase, MergeDatabaseWithSingleRegionTooManyNewTiles) {
    FixtureLog log;
    util::deleteFile(filename_sideload);
    util::copyFile(filename_sideload, "test/fixtures/offline_database/sideload_sat_multiple.db");

    OfflineDatabase db(":memory:");
    db.setOfflineMapboxTileCountLimit(1);

    auto result = db.mergeDatabase(filename_sideload);
    EXPECT_FALSE(result);
    EXPECT_EQ(1u, log.count({ EventSeverity::Error, Event::Database, -1, "Mapbox tile limit exceeded" }));
    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, MergeDatabaseWithSingleRegionTooManyExistingTiles) {
    FixtureLog log;
    deleteDatabaseFiles();
    util::deleteFile(filename_sideload);
    util::copyFile(filename, "test/fixtures/offline_database/sideload_sat_multiple.db");
    util::copyFile(filename_sideload, "test/fixtures/offline_database/satellite_test.db");

    OfflineDatabase db(filename);
    db.setOfflineMapboxTileCountLimit(2);

    auto result = db.mergeDatabase(filename_sideload);
    EXPECT_THROW(std::rethrow_exception(result.error()), MapboxTileLimitExceededException);

    EXPECT_EQ(1u, log.count({ EventSeverity::Error, Event::Database, -1, "Mapbox tile limit exceeded" }));
    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, MergeDatabaseWithInvalidPath) {
    FixtureLog log;

    util::deleteFile(filename_sideload);
    util::copyFile(filename_sideload, "test/fixtures/offline_database");

    OfflineDatabase db(":memory:");

    auto result = db.mergeDatabase(filename_sideload);
    EXPECT_FALSE(result);

    EXPECT_EQ(1u, log.count({ EventSeverity::Error, Event::Database, -1, "Merge database has incorrect user_version" }));
    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(OfflineDatabase, MergeDatabaseWithInvalidDb) {
    FixtureLog log;

    util::deleteFile(filename_sideload);
    util::copyFile(filename_sideload, "test/fixtures/offline_database/corrupt-immediate.db");

    OfflineDatabase db(":memory:");

    auto result = db.mergeDatabase(filename_sideload);
    EXPECT_FALSE(result);

    EXPECT_EQ(1u, log.count(error(ResultCode::Corrupt, "Can't attach database (test/fixtures/offline_database/offline_sideload.db) for merge: database disk image is malformed")));
    EXPECT_EQ(0u, log.uncheckedCount());
}

#ifndef __QT__ // Qt doesn't expose the ability to register virtual file system handlers.
TEST(OfflineDatabase, TEST_REQUIRES_WRITE(MergeDatabaseWithDiskFull)) {
    FixtureLog log;
    test::SQLite3TestFS fs;
    
    deleteDatabaseFiles();
    util::deleteFile(filename_sideload);
    util::copyFile(filename, "test/fixtures/offline_database/satellite_test.db");
    util::copyFile(filename_sideload, "test/fixtures/offline_database/sideload_sat.db");

    OfflineDatabase db(filename_test_fs);

    fs.setWriteLimit(0);

    auto result = db.mergeDatabase(filename_sideload);
    EXPECT_FALSE(result.has_value());

    EXPECT_EQ(1u, log.count({ EventSeverity::Error, Event::Database, -1, "database or disk is full"}));
    EXPECT_EQ(0u, log.uncheckedCount());
}
#endif // __QT__

TEST(OfflineDatabase, ChangePath) {
    std::string newPath("test/fixtures/offline_database/test.db");
    OfflineDatabase db(":memory:");
    db.changePath(newPath);
    mapbox::sqlite::Database::open(newPath, mapbox::sqlite::ReadOnly);
    util::deleteFile(newPath);
}

TEST(OfflineDatabase, ResetDatabase) {
    FixtureLog log;
    deleteDatabaseFiles();
    util::copyFile(filename, "test/fixtures/offline_database/satellite_test.db");

    OfflineDatabase db(filename);
    auto result = db.resetDatabase();
    EXPECT_FALSE(result);

    auto regions = db.listRegions().value();
    EXPECT_EQ(0u, regions.size());
    EXPECT_EQ(1u, log.count({ EventSeverity::Warning, Event::Database, -1, "Removing existing incompatible offline database" }));
    EXPECT_EQ(0u, log.uncheckedCount());
}
