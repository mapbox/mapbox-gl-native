#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/compression.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/logging.hpp>

#include "offline_schema.hpp"
#include "merge_sideloaded.hpp"

#include "sqlite3.hpp"

namespace mbgl {

OfflineDatabase::OfflineDatabase(std::string path_, uint64_t maximumCacheSize_)
    : path(std::move(path_)),
      maximumCacheSize(maximumCacheSize_) {
    try {
        initialize();
    } catch (const util::IOException& ex) {
        handleError(ex, "open database");
    } catch (const mapbox::sqlite::Exception& ex) {
        handleError(ex, "open database");
    }
    // Assume that we can't open the database right now and work with an empty database object.
}

OfflineDatabase::~OfflineDatabase() {
    // Deleting these SQLite objects may result in exceptions, but we're in a destructor, so we
    // can't throw anything.
    try {
        statements.clear();
        db.reset();
    } catch (const util::IOException& ex) {
        handleError(ex, "close database");
    } catch (const mapbox::sqlite::Exception& ex) {
        handleError(ex, "close database");
    }
}

void OfflineDatabase::initialize() {
    assert(!db);
    assert(statements.empty());

    db = std::make_unique<mapbox::sqlite::Database>(
        mapbox::sqlite::Database::open(path, mapbox::sqlite::ReadWriteCreate));
    db->setBusyTimeout(Milliseconds::max());
    db->exec("PRAGMA foreign_keys = ON");

    const auto userVersion = getPragma<int64_t>("PRAGMA user_version");
    switch (userVersion) {
    case 0:
    case 1:
        // Newly created database, or old cache-only database; remove old table if it exists.
        removeOldCacheTable();
        createSchema();
        return;
    case 2:
        migrateToVersion3();
        // fall through
    case 3:
        // Removed migration, see below.
        // fall through
    case 4:
        migrateToVersion5();
        // fall through
    case 5:
        migrateToVersion6();
        // fall through
    case 6:
        // Happy path; we're done
        return;
    default:
        // Downgrade: delete the database and try to reinitialize.
        removeExisting();
        initialize();
    }
}

void OfflineDatabase::handleError(const mapbox::sqlite::Exception& ex, const char* action) {
    if (ex.code == mapbox::sqlite::ResultCode::NotADB ||
        ex.code == mapbox::sqlite::ResultCode::Corrupt ||
        (ex.code == mapbox::sqlite::ResultCode::ReadOnly &&
         ex.extendedCode == mapbox::sqlite::ExtendedResultCode::ReadOnlyDBMoved)) {
        // The database was corruped, moved away, or deleted. We're going to start fresh with a
        // clean slate for the next operation.
        Log::Error(Event::Database, static_cast<int>(ex.code), "Can't %s: %s", action, ex.what());
        try {
            removeExisting();
        } catch (const util::IOException& ioEx) {
            handleError(ioEx, action);
        }
    } else {
        // We treat the error as temporary, and pretend we have an inaccessible DB.
        Log::Warning(Event::Database, static_cast<int>(ex.code), "Can't %s: %s", action, ex.what());
    }
}

void OfflineDatabase::handleError(const util::IOException& ex, const char* action) {
    // We failed to delete the database file.
    Log::Error(Event::Database, ex.code, "Can't %s: %s", action, ex.what());
}

void OfflineDatabase::removeExisting() {
    Log::Warning(Event::Database, "Removing existing incompatible offline database");

    statements.clear();
    db.reset();

    util::deleteFile(path);
}

void OfflineDatabase::removeOldCacheTable() {
    assert(db);
    db->exec("DROP TABLE IF EXISTS http_cache");
    db->exec("VACUUM");
}

void OfflineDatabase::createSchema() {
    assert(db);
    db->exec("PRAGMA auto_vacuum = INCREMENTAL");
    db->exec("PRAGMA journal_mode = DELETE");
    db->exec("PRAGMA synchronous = FULL");
    mapbox::sqlite::Transaction transaction(*db);
    db->exec(offlineDatabaseSchema);
    db->exec("PRAGMA user_version = 6");
    transaction.commit();
}

void OfflineDatabase::migrateToVersion3() {
    assert(db);
    db->exec("PRAGMA auto_vacuum = INCREMENTAL");
    db->exec("VACUUM");
    db->exec("PRAGMA user_version = 3");
}

// Schema version 4 was WAL journal + NORMAL sync. It was reverted during pre-
// release development and the migration was removed entirely to avoid potential
// conflicts from quickly (and needlessly) switching journal and sync modes.
//
// See: https://github.com/mapbox/mapbox-gl-native/pull/6320

void OfflineDatabase::migrateToVersion5() {
    assert(db);
    db->exec("PRAGMA journal_mode = DELETE");
    db->exec("PRAGMA synchronous = FULL");
    db->exec("PRAGMA user_version = 5");
}

void OfflineDatabase::migrateToVersion6() {
    assert(db);
    mapbox::sqlite::Transaction transaction(*db);
    db->exec("ALTER TABLE resources ADD COLUMN must_revalidate INTEGER NOT NULL DEFAULT 0");
    db->exec("ALTER TABLE tiles ADD COLUMN must_revalidate INTEGER NOT NULL DEFAULT 0");
    db->exec("PRAGMA user_version = 6");
    transaction.commit();
}

mapbox::sqlite::Statement& OfflineDatabase::getStatement(const char* sql) {
    if (!db) {
        initialize();
    }
    auto it = statements.find(sql);
    if (it == statements.end()) {
        it = statements.emplace(sql, std::make_unique<mapbox::sqlite::Statement>(*db, sql)).first;
    }
    return *it->second;
}

optional<Response> OfflineDatabase::get(const Resource& resource) try {
    auto result = getInternal(resource);
    return result ? optional<Response>{ result->first } : nullopt;
} catch (const util::IOException& ex) {
    handleError(ex, "read resource");
    return nullopt;
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "read resource");
    return nullopt;
}

optional<std::pair<Response, uint64_t>> OfflineDatabase::getInternal(const Resource& resource) {
    if (resource.kind == Resource::Kind::Tile) {
        assert(resource.tileData);
        return getTile(*resource.tileData);
    } else {
        return getResource(resource);
    }
}

optional<int64_t> OfflineDatabase::hasInternal(const Resource& resource) {
    if (resource.kind == Resource::Kind::Tile) {
        assert(resource.tileData);
        return hasTile(*resource.tileData);
    } else {
        return hasResource(resource);
    }
}

std::pair<bool, uint64_t> OfflineDatabase::put(const Resource& resource, const Response& response) try {
    if (!db) {
        initialize();
    }
    mapbox::sqlite::Transaction transaction(*db, mapbox::sqlite::Transaction::Immediate);
    auto result = putInternal(resource, response, true);
    transaction.commit();
    return result;
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "write resource");
    return { false, 0 };
}

std::pair<bool, uint64_t> OfflineDatabase::putInternal(const Resource& resource, const Response& response, bool evict_) {
    if (response.error) {
        return { false, 0 };
    }

    std::string compressedData;
    bool compressed = false;
    uint64_t size = 0;

    if (response.data) {
        compressedData = util::compress(*response.data);
        compressed = compressedData.size() < response.data->size();
        size = compressed ? compressedData.size() : response.data->size();
    }

    if (evict_ && !evict(size)) {
        Log::Info(Event::Database, "Unable to make space for entry");
        return { false, 0 };
    }

    bool inserted;

    if (resource.kind == Resource::Kind::Tile) {
        assert(resource.tileData);
        inserted = putTile(*resource.tileData, response,
                compressed ? compressedData : response.data ? *response.data : "",
                compressed);
    } else {
        inserted = putResource(resource, response,
                compressed ? compressedData : response.data ? *response.data : "",
                compressed);
    }

    return { inserted, size };
}

optional<std::pair<Response, uint64_t>> OfflineDatabase::getResource(const Resource& resource) {
    // Update accessed timestamp used for LRU eviction.
    try {
        mapbox::sqlite::Query accessedQuery{ getStatement("UPDATE resources SET accessed = ?1 WHERE url = ?2") };
        accessedQuery.bind(1, util::now());
        accessedQuery.bind(2, resource.url);
        accessedQuery.run();
    } catch (const mapbox::sqlite::Exception& ex) {
        if (ex.code == mapbox::sqlite::ResultCode::NotADB ||
            ex.code == mapbox::sqlite::ResultCode::Corrupt) {
            throw;
        }

        // If we don't have any indication that the database is corrupt, continue as usual.
        Log::Warning(Event::Database, static_cast<int>(ex.code), "Can't update timestamp: %s", ex.what());
    }

    // clang-format off
    mapbox::sqlite::Query query{ getStatement(
        //        0      1            2            3       4      5
        "SELECT etag, expires, must_revalidate, modified, data, compressed "
        "FROM resources "
        "WHERE url = ?") };
    // clang-format on

    query.bind(1, resource.url);

    if (!query.run()) {
        return nullopt;
    }

    Response response;
    uint64_t size = 0;

    response.etag           = query.get<optional<std::string>>(0);
    response.expires        = query.get<optional<Timestamp>>(1);
    response.mustRevalidate = query.get<bool>(2);
    response.modified       = query.get<optional<Timestamp>>(3);

    auto data = query.get<optional<std::string>>(4);
    if (!data) {
        response.noContent = true;
    } else if (query.get<bool>(5)) {
        response.data = std::make_shared<std::string>(util::decompress(*data));
        size = data->length();
    } else {
        response.data = std::make_shared<std::string>(*data);
        size = data->length();
    }

    return std::make_pair(response, size);
}

optional<int64_t> OfflineDatabase::hasResource(const Resource& resource) {
    mapbox::sqlite::Query query{ getStatement("SELECT length(data) FROM resources WHERE url = ?") };
    query.bind(1, resource.url);
    if (!query.run()) {
        return nullopt;
    }

    return query.get<optional<int64_t>>(0);
}

bool OfflineDatabase::putResource(const Resource& resource,
                                  const Response& response,
                                  const std::string& data,
                                  bool compressed) {
    if (response.notModified) {
        // clang-format off
        mapbox::sqlite::Query notModifiedQuery{ getStatement(
            "UPDATE resources "
            "SET accessed         = ?1, "
            "    expires          = ?2, "
            "    must_revalidate  = ?3 "
            "WHERE url    = ?4 ") };
        // clang-format on

        notModifiedQuery.bind(1, util::now());
        notModifiedQuery.bind(2, response.expires);
        notModifiedQuery.bind(3, response.mustRevalidate);
        notModifiedQuery.bind(4, resource.url);
        notModifiedQuery.run();
        return false;
    }

    // We can't use REPLACE because it would change the id value.
    // clang-format off
    mapbox::sqlite::Query updateQuery{ getStatement(
        "UPDATE resources "
        "SET kind            = ?1, "
        "    etag            = ?2, "
        "    expires         = ?3, "
        "    must_revalidate = ?4, "
        "    modified        = ?5, "
        "    accessed        = ?6, "
        "    data            = ?7, "
        "    compressed      = ?8 "
        "WHERE url           = ?9 ") };
    // clang-format on

    updateQuery.bind(1, int(resource.kind));
    updateQuery.bind(2, response.etag);
    updateQuery.bind(3, response.expires);
    updateQuery.bind(4, response.mustRevalidate);
    updateQuery.bind(5, response.modified);
    updateQuery.bind(6, util::now());
    updateQuery.bind(9, resource.url);

    if (response.noContent) {
        updateQuery.bind(7, nullptr);
        updateQuery.bind(8, false);
    } else {
        updateQuery.bindBlob(7, data.data(), data.size(), false);
        updateQuery.bind(8, compressed);
    }

    updateQuery.run();
    if (updateQuery.changes() != 0) {
        return false;
    }

    // clang-format off
    mapbox::sqlite::Query insertQuery{ getStatement(
        "INSERT INTO resources (url, kind, etag, expires, must_revalidate, modified, accessed, data, compressed) "
        "VALUES                (?1,  ?2,   ?3,   ?4,      ?5,              ?6,       ?7,       ?8,   ?9) ") };
    // clang-format on

    insertQuery.bind(1, resource.url);
    insertQuery.bind(2, int(resource.kind));
    insertQuery.bind(3, response.etag);
    insertQuery.bind(4, response.expires);
    insertQuery.bind(5, response.mustRevalidate);
    insertQuery.bind(6, response.modified);
    insertQuery.bind(7, util::now());

    if (response.noContent) {
        insertQuery.bind(8, nullptr);
        insertQuery.bind(9, false);
    } else {
        insertQuery.bindBlob(8, data.data(), data.size(), false);
        insertQuery.bind(9, compressed);
    }

    insertQuery.run();

    return true;
}

optional<std::pair<Response, uint64_t>> OfflineDatabase::getTile(const Resource::TileData& tile) {
    // Update accessed timestamp used for LRU eviction.
    try {
        // clang-format off
        mapbox::sqlite::Query accessedQuery{ getStatement(
            "UPDATE tiles "
            "SET accessed       = ?1 "
            "WHERE url_template = ?2 "
            "  AND pixel_ratio  = ?3 "
            "  AND x            = ?4 "
            "  AND y            = ?5 "
            "  AND z            = ?6 ") };
        // clang-format on

        accessedQuery.bind(1, util::now());
        accessedQuery.bind(2, tile.urlTemplate);
        accessedQuery.bind(3, tile.pixelRatio);
        accessedQuery.bind(4, tile.x);
        accessedQuery.bind(5, tile.y);
        accessedQuery.bind(6, tile.z);
        accessedQuery.run();
    } catch (const mapbox::sqlite::Exception& ex) {
        if (ex.code == mapbox::sqlite::ResultCode::NotADB || ex.code == mapbox::sqlite::ResultCode::Corrupt) {
            throw;
        }

        // If we don't have any indication that the database is corrupt, continue as usual.
        Log::Warning(Event::Database, static_cast<int>(ex.code), "Can't update timestamp: %s", ex.what());
    }

    // clang-format off
    mapbox::sqlite::Query query{ getStatement(
        //        0      1           2,            3,      4,      5
        "SELECT etag, expires, must_revalidate, modified, data, compressed "
        "FROM tiles "
        "WHERE url_template = ?1 "
        "  AND pixel_ratio  = ?2 "
        "  AND x            = ?3 "
        "  AND y            = ?4 "
        "  AND z            = ?5 ") };
    // clang-format on

    query.bind(1, tile.urlTemplate);
    query.bind(2, tile.pixelRatio);
    query.bind(3, tile.x);
    query.bind(4, tile.y);
    query.bind(5, tile.z);

    if (!query.run()) {
        return nullopt;
    }

    Response response;
    uint64_t size = 0;

    response.etag            = query.get<optional<std::string>>(0);
    response.expires         = query.get<optional<Timestamp>>(1);
    response.mustRevalidate  = query.get<bool>(2);
    response.modified        = query.get<optional<Timestamp>>(3);

    optional<std::string> data = query.get<optional<std::string>>(4);
    if (!data) {
        response.noContent = true;
    } else if (query.get<bool>(5)) {
        response.data = std::make_shared<std::string>(util::decompress(*data));
        size = data->length();
    } else {
        response.data = std::make_shared<std::string>(*data);
        size = data->length();
    }

    return std::make_pair(response, size);
}

optional<int64_t> OfflineDatabase::hasTile(const Resource::TileData& tile) {
    // clang-format off
    mapbox::sqlite::Query size{ getStatement(
        "SELECT length(data) "
        "FROM tiles "
        "WHERE url_template = ?1 "
        "  AND pixel_ratio  = ?2 "
        "  AND x            = ?3 "
        "  AND y            = ?4 "
        "  AND z            = ?5 ") };
    // clang-format on

    size.bind(1, tile.urlTemplate);
    size.bind(2, tile.pixelRatio);
    size.bind(3, tile.x);
    size.bind(4, tile.y);
    size.bind(5, tile.z);

    if (!size.run()) {
        return nullopt;
    }

    return size.get<optional<int64_t>>(0);
}

bool OfflineDatabase::putTile(const Resource::TileData& tile,
                              const Response& response,
                              const std::string& data,
                              bool compressed) {
    if (response.notModified) {
        // clang-format off
        mapbox::sqlite::Query notModifiedQuery{ getStatement(
            "UPDATE tiles "
            "SET accessed        = ?1, "
            "    expires         = ?2, "
            "    must_revalidate = ?3 "
            "WHERE url_template  = ?4 "
            "  AND pixel_ratio   = ?5 "
            "  AND x             = ?6 "
            "  AND y             = ?7 "
            "  AND z             = ?8 ") };
        // clang-format on

        notModifiedQuery.bind(1, util::now());
        notModifiedQuery.bind(2, response.expires);
        notModifiedQuery.bind(3, response.mustRevalidate);
        notModifiedQuery.bind(4, tile.urlTemplate);
        notModifiedQuery.bind(5, tile.pixelRatio);
        notModifiedQuery.bind(6, tile.x);
        notModifiedQuery.bind(7, tile.y);
        notModifiedQuery.bind(8, tile.z);
        notModifiedQuery.run();
        return false;
    }

    // We can't use REPLACE because it would change the id value.

    // clang-format off
    mapbox::sqlite::Query updateQuery{ getStatement(
        "UPDATE tiles "
        "SET modified        = ?1, "
        "    etag            = ?2, "
        "    expires         = ?3, "
        "    must_revalidate = ?4, "
        "    accessed        = ?5, "
        "    data            = ?6, "
        "    compressed      = ?7 "
        "WHERE url_template  = ?8 "
        "  AND pixel_ratio   = ?9 "
        "  AND x             = ?10 "
        "  AND y             = ?11 "
        "  AND z             = ?12 ") };
    // clang-format on

    updateQuery.bind(1, response.modified);
    updateQuery.bind(2, response.etag);
    updateQuery.bind(3, response.expires);
    updateQuery.bind(4, response.mustRevalidate);
    updateQuery.bind(5, util::now());
    updateQuery.bind(8, tile.urlTemplate);
    updateQuery.bind(9, tile.pixelRatio);
    updateQuery.bind(10, tile.x);
    updateQuery.bind(11, tile.y);
    updateQuery.bind(12, tile.z);

    if (response.noContent) {
        updateQuery.bind(6, nullptr);
        updateQuery.bind(7, false);
    } else {
        updateQuery.bindBlob(6, data.data(), data.size(), false);
        updateQuery.bind(7, compressed);
    }

    updateQuery.run();
    if (updateQuery.changes() != 0) {
        return false;
    }

    // clang-format off
    mapbox::sqlite::Query insertQuery{ getStatement(
        "INSERT INTO tiles (url_template, pixel_ratio, x,  y,  z,  modified, must_revalidate, etag, expires, accessed,  data, compressed) "
        "VALUES            (?1,           ?2,          ?3, ?4, ?5, ?6,       ?7,              ?8,   ?9,      ?10,       ?11,  ?12)") };
    // clang-format on

    insertQuery.bind(1, tile.urlTemplate);
    insertQuery.bind(2, tile.pixelRatio);
    insertQuery.bind(3, tile.x);
    insertQuery.bind(4, tile.y);
    insertQuery.bind(5, tile.z);
    insertQuery.bind(6, response.modified);
    insertQuery.bind(7, response.mustRevalidate);
    insertQuery.bind(8, response.etag);
    insertQuery.bind(9, response.expires);
    insertQuery.bind(10, util::now());

    if (response.noContent) {
        insertQuery.bind(11, nullptr);
        insertQuery.bind(12, false);
    } else {
        insertQuery.bindBlob(11, data.data(), data.size(), false);
        insertQuery.bind(12, compressed);
    }

    insertQuery.run();

    return true;
}

expected<OfflineRegions, std::exception_ptr> OfflineDatabase::listRegions() try {
    mapbox::sqlite::Query query{ getStatement("SELECT id, definition, description FROM regions") };
    OfflineRegions result;
    while (query.run()) {
        const auto id = query.get<int64_t>(0);
        const auto definition = query.get<std::string>(1);
        const auto description = query.get<std::vector<uint8_t>>(2);
        try {
            // Construct, then move because this constructor is private.
            OfflineRegion region(id, decodeOfflineRegionDefinition(definition), description);
            result.emplace_back(std::move(region));
        } catch (const std::exception& ex) {
            // Catch errors from malformed offline region definitions
            // and skip them.
            Log::Error(Event::General, "%s", ex.what());
        }
    }
    // Explicit move to avoid triggering the copy constructor.
    return { std::move(result) };
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "list regions");
    return unexpected<std::exception_ptr>(std::current_exception());
}

expected<OfflineRegion, std::exception_ptr>
OfflineDatabase::createRegion(const OfflineRegionDefinition& definition,
                              const OfflineRegionMetadata& metadata) try {
    // clang-format off
    mapbox::sqlite::Query query{ getStatement(
        "INSERT INTO regions (definition, description) "
        "VALUES              (?1,         ?2) ") };
    // clang-format on

    query.bind(1, encodeOfflineRegionDefinition(definition));
    query.bindBlob(2, metadata);
    query.run();
    return OfflineRegion(query.lastInsertRowId(), definition, metadata);
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "create region");
    return unexpected<std::exception_ptr>(std::current_exception());
}

expected<OfflineRegions, std::exception_ptr>
OfflineDatabase::mergeDatabase(const std::string& sideDatabasePath) {
    try {
        // clang-format off
        mapbox::sqlite::Query query{ getStatement("ATTACH DATABASE ?1 AS side") };
        // clang-format on

        query.bind(1, sideDatabasePath);
        query.run();
    } catch (const mapbox::sqlite::Exception& ex) {
        handleError(ex, "merge databse attach");
        return unexpected<std::exception_ptr>(std::current_exception());
    }
    try {
        // Attaching an accessible path without a db file creates a new temporary
        //database and attaches it. Check for matching schema version.
        auto sideUserVersion = static_cast<int>(getPragma<int64_t>("PRAGMA side.user_version"));
        if (sideUserVersion != 6) {
            Log::Warning(Event::Database, "Merge database does not match user_version of main database");
            throw std::runtime_error("merge database does not match schema or has incorrect user_version");
        }

        mapbox::sqlite::Transaction transaction(*db);
        db->exec(mergeSideloadedDatabaseSQL);
        transaction.commit();

        // clang-format off
        mapbox::sqlite::Query query{ getStatement(
            "SELECT r.id, r.definition, r.description "
            "FROM side.regions sr "
            "JOIN regions r ON sr.definition = r.definition") };
        // clang-format on

        OfflineRegions result;
        while (query.run()) {
            // Construct, then move because this constructor is private.
            OfflineRegion region(query.get<int64_t>(0),
                decodeOfflineRegionDefinition(query.get<std::string>(1)),
                query.get<std::vector<uint8_t>>(2));
            result.emplace_back(std::move(region));
        }
        db->exec("DETACH DATABASE side");
        // Explicit move to avoid triggering the copy constructor.
        return { std::move(result) };
    } catch (const mapbox::sqlite::Exception& ex) {
        db->exec("DETACH DATABASE side");
        handleError(ex, "merge databse post merge sql");
        return unexpected<std::exception_ptr>(std::current_exception());
    }
    return {};
}

expected<OfflineRegionMetadata, std::exception_ptr>
OfflineDatabase::updateMetadata(const int64_t regionID, const OfflineRegionMetadata& metadata) try {
    // clang-format off
    mapbox::sqlite::Query query{ getStatement(
                                  "UPDATE regions SET description = ?1 "
                                  "WHERE id = ?2") };
    // clang-format on
    query.bindBlob(1, metadata);
    query.bind(2, regionID);
    query.run();

    return metadata;
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "update region metadata");
    return unexpected<std::exception_ptr>(std::current_exception());
}

std::exception_ptr OfflineDatabase::deleteRegion(OfflineRegion&& region) try {
    {
        mapbox::sqlite::Query query{ getStatement("DELETE FROM regions WHERE id = ?") };
        query.bind(1, region.getID());
        query.run();
    }

    evict(0);
    assert(db);
    db->exec("PRAGMA incremental_vacuum");

    // Ensure that the cached offlineTileCount value is recalculated.
    offlineMapboxTileCount = {};
    return nullptr;
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "delete region");
    return std::current_exception();
}

optional<std::pair<Response, uint64_t>> OfflineDatabase::getRegionResource(int64_t regionID, const Resource& resource) try {
    auto response = getInternal(resource);

    if (response) {
        markUsed(regionID, resource);
    }

    return response;
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "read region resource");
    return nullopt;
}

optional<int64_t> OfflineDatabase::hasRegionResource(int64_t regionID, const Resource& resource) try {
    auto response = hasInternal(resource);

    if (response) {
        markUsed(regionID, resource);
    }

    return response;
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "query region resource");
    return nullopt;
}

uint64_t OfflineDatabase::putRegionResource(int64_t regionID,
                                            const Resource& resource,
                                            const Response& response) try {
    if (!db) {
        initialize();
    }
    mapbox::sqlite::Transaction transaction(*db);
    auto size = putRegionResourceInternal(regionID, resource, response);
    transaction.commit();
    return size;
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "write region resource");
    return 0;
}

void OfflineDatabase::putRegionResources(int64_t regionID,
                                         const std::list<std::tuple<Resource, Response>>& resources,
                                         OfflineRegionStatus& status) try {
    if (!db) {
        initialize();
    }
    mapbox::sqlite::Transaction transaction(*db);

    // Accumulate all statistics locally first before adding them to the OfflineRegionStatus object
    // to ensure correctness when the transaction fails.
    uint64_t completedResourceCount = 0;
    uint64_t completedResourceSize = 0;
    uint64_t completedTileCount = 0;
    uint64_t completedTileSize = 0;

    for (const auto& elem : resources) {
        const auto& resource = std::get<0>(elem);
        const auto& response = std::get<1>(elem);

        try {
            uint64_t resourceSize = putRegionResourceInternal(regionID, resource, response);
            completedResourceCount++;
            completedResourceSize += resourceSize;
            if (resource.kind == Resource::Kind::Tile) {
                completedTileCount += 1;
                completedTileSize += resourceSize;
            }
        } catch (const MapboxTileLimitExceededException&) {
            // Commit the rest of the batch and retrow
            transaction.commit();
            throw;
        }
    }

    // Commit the completed batch
    transaction.commit();

    status.completedResourceCount += completedResourceCount;
    status.completedResourceSize += completedResourceSize;
    status.completedTileCount += completedTileCount;
    status.completedTileSize += completedTileSize;
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "write region resources");
}

uint64_t OfflineDatabase::putRegionResourceInternal(int64_t regionID, const Resource& resource, const Response& response) {
    if (exceedsOfflineMapboxTileCountLimit(resource)) {
        throw MapboxTileLimitExceededException();
    }

    uint64_t size = putInternal(resource, response, false).second;
    bool previouslyUnused = markUsed(regionID, resource);

    if (offlineMapboxTileCount
        && resource.kind == Resource::Kind::Tile
        && util::mapbox::isMapboxURL(resource.url)
        && previouslyUnused) {
        *offlineMapboxTileCount += 1;
    }

    return size;
}

bool OfflineDatabase::markUsed(int64_t regionID, const Resource& resource) {
    if (resource.kind == Resource::Kind::Tile) {
        // clang-format off
        mapbox::sqlite::Query insertQuery{ getStatement(
            "INSERT OR IGNORE INTO region_tiles (region_id, tile_id) "
            "SELECT                              ?1,        tiles.id "
            "FROM tiles "
            "WHERE url_template = ?2 "
            "  AND pixel_ratio  = ?3 "
            "  AND x            = ?4 "
            "  AND y            = ?5 "
            "  AND z            = ?6 ") };
        // clang-format on

        const Resource::TileData& tile = *resource.tileData;
        insertQuery.bind(1, regionID);
        insertQuery.bind(2, tile.urlTemplate);
        insertQuery.bind(3, tile.pixelRatio);
        insertQuery.bind(4, tile.x);
        insertQuery.bind(5, tile.y);
        insertQuery.bind(6, tile.z);
        insertQuery.run();

        if (insertQuery.changes() == 0) {
            return false;
        }

        // clang-format off
        mapbox::sqlite::Query selectQuery{ getStatement(
            "SELECT region_id "
            "FROM region_tiles, tiles "
            "WHERE region_id   != ?1 "
            "  AND url_template = ?2 "
            "  AND pixel_ratio  = ?3 "
            "  AND x            = ?4 "
            "  AND y            = ?5 "
            "  AND z            = ?6 "
            "LIMIT 1 ") };
        // clang-format on

        selectQuery.bind(1, regionID);
        selectQuery.bind(2, tile.urlTemplate);
        selectQuery.bind(3, tile.pixelRatio);
        selectQuery.bind(4, tile.x);
        selectQuery.bind(5, tile.y);
        selectQuery.bind(6, tile.z);
        return !selectQuery.run();
    } else {
        // clang-format off
        mapbox::sqlite::Query insertQuery{ getStatement(
            "INSERT OR IGNORE INTO region_resources (region_id, resource_id) "
            "SELECT                                  ?1,        resources.id "
            "FROM resources "
            "WHERE resources.url = ?2 ") };
        // clang-format on

        insertQuery.bind(1, regionID);
        insertQuery.bind(2, resource.url);
        insertQuery.run();

        if (insertQuery.changes() == 0) {
            return false;
        }

        // clang-format off
        mapbox::sqlite::Query selectQuery{ getStatement(
            "SELECT region_id "
            "FROM region_resources, resources "
            "WHERE region_id    !=  ?1 "
            "  AND resources.url = ?2 "
            "LIMIT 1 ") };
        // clang-format on

        selectQuery.bind(1, regionID);
        selectQuery.bind(2, resource.url);
        return !selectQuery.run();
    }
}

expected<OfflineRegionDefinition, std::exception_ptr> OfflineDatabase::getRegionDefinition(int64_t regionID) try {
    mapbox::sqlite::Query query{ getStatement("SELECT definition FROM regions WHERE id = ?1") };
    query.bind(1, regionID);
    query.run();

    return decodeOfflineRegionDefinition(query.get<std::string>(0));
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "load region");
    return unexpected<std::exception_ptr>(std::current_exception());
}

expected<OfflineRegionStatus, std::exception_ptr> OfflineDatabase::getRegionCompletedStatus(int64_t regionID) try {
    OfflineRegionStatus result;

    std::tie(result.completedResourceCount, result.completedResourceSize)
        = getCompletedResourceCountAndSize(regionID);
    std::tie(result.completedTileCount, result.completedTileSize)
        = getCompletedTileCountAndSize(regionID);

    result.completedResourceCount += result.completedTileCount;
    result.completedResourceSize += result.completedTileSize;

    return result;
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "get region status");
    return unexpected<std::exception_ptr>(std::current_exception());
}

std::pair<int64_t, int64_t> OfflineDatabase::getCompletedResourceCountAndSize(int64_t regionID) {
    // clang-format off
    mapbox::sqlite::Query query{ getStatement(
        "SELECT COUNT(*), SUM(LENGTH(data)) "
        "FROM region_resources, resources "
        "WHERE region_id = ?1 "
        "AND resource_id = resources.id ") };
    // clang-format on
    query.bind(1, regionID);
    query.run();
    return { query.get<int64_t>(0), query.get<int64_t>(1) };
}

std::pair<int64_t, int64_t> OfflineDatabase::getCompletedTileCountAndSize(int64_t regionID) {
    // clang-format off
    mapbox::sqlite::Query query{ getStatement(
        "SELECT COUNT(*), SUM(LENGTH(data)) "
        "FROM region_tiles, tiles "
        "WHERE region_id = ?1 "
        "AND tile_id = tiles.id ") };
    // clang-format on
    query.bind(1, regionID);
    query.run();
    return { query.get<int64_t>(0), query.get<int64_t>(1) };
}

template <class T>
T OfflineDatabase::getPragma(const char* sql) {
    mapbox::sqlite::Query query{ getStatement(sql) };
    query.run();
    return query.get<T>(0);
}

// Remove least-recently used resources and tiles until the used database size,
// as calculated by multiplying the number of in-use pages by the page size, is
// less than the maximum cache size. Returns false if this condition cannot be
// satisfied.
//
// SQLite database never shrinks in size unless we call VACCUM. We here
// are monitoring the soft limit (i.e. number of free pages in the file)
// and as it approaches to the hard limit (i.e. the actual file size) we
// delete an arbitrary number of old cache entries. The free pages approach saves
// us from calling VACCUM or keeping a running total, which can be costly.
bool OfflineDatabase::evict(uint64_t neededFreeSize) {
    uint64_t pageSize = getPragma<int64_t>("PRAGMA page_size");
    uint64_t pageCount = getPragma<int64_t>("PRAGMA page_count");

    auto usedSize = [&] {
        return pageSize * (pageCount - getPragma<int64_t>("PRAGMA freelist_count"));
    };

    // The addition of pageSize is a fudge factor to account for non `data` column
    // size, and because pages can get fragmented on the database.
    while (usedSize() + neededFreeSize + pageSize > maximumCacheSize) {
        // clang-format off
        mapbox::sqlite::Query accessedQuery{ getStatement(
            "SELECT max(accessed) "
            "FROM ( "
            "    SELECT accessed "
            "    FROM resources "
            "    LEFT JOIN region_resources "
            "    ON resource_id = resources.id "
            "    WHERE resource_id IS NULL "
            "  UNION ALL "
            "    SELECT accessed "
            "    FROM tiles "
            "    LEFT JOIN region_tiles "
            "    ON tile_id = tiles.id "
            "    WHERE tile_id IS NULL "
            "  ORDER BY accessed ASC LIMIT ?1 "
            ") "
        ) };
        accessedQuery.bind(1, 50);
        // clang-format on
        if (!accessedQuery.run()) {
            return false;
        }
        Timestamp accessed = accessedQuery.get<Timestamp>(0);

        // clang-format off
        mapbox::sqlite::Query resourceQuery{ getStatement(
            "DELETE FROM resources "
            "WHERE id IN ( "
            "  SELECT id FROM resources "
            "  LEFT JOIN region_resources "
            "  ON resource_id = resources.id "
            "  WHERE resource_id IS NULL "
            "  AND accessed <= ?1 "
            ") ") };
        // clang-format on
        resourceQuery.bind(1, accessed);
        resourceQuery.run();
        const uint64_t resourceChanges = resourceQuery.changes();

        // clang-format off
        mapbox::sqlite::Query tileQuery{ getStatement(
            "DELETE FROM tiles "
            "WHERE id IN ( "
            "  SELECT id FROM tiles "
            "  LEFT JOIN region_tiles "
            "  ON tile_id = tiles.id "
            "  WHERE tile_id IS NULL "
            "  AND accessed <= ?1 "
            ") ") };
        // clang-format on
        tileQuery.bind(1, accessed);
        tileQuery.run();
        const uint64_t tileChanges = tileQuery.changes();

        // The cached value of offlineTileCount does not need to be updated
        // here because only non-offline tiles can be removed by eviction.

        if (resourceChanges == 0 && tileChanges == 0) {
            return false;
        }
    }

    return true;
}

void OfflineDatabase::setOfflineMapboxTileCountLimit(uint64_t limit) {
    offlineMapboxTileCountLimit = limit;
}

uint64_t OfflineDatabase::getOfflineMapboxTileCountLimit() {
    return offlineMapboxTileCountLimit;
}

bool OfflineDatabase::offlineMapboxTileCountLimitExceeded() {
    return getOfflineMapboxTileCount() >= offlineMapboxTileCountLimit;
}

uint64_t OfflineDatabase::getOfflineMapboxTileCount() try {
    // Calculating this on every call would be much simpler than caching and
    // manually updating the value, but it would make offline downloads an O(n²)
    // operation, because the database query below involves an index scan of
    // region_tiles.

    if (offlineMapboxTileCount) {
        return *offlineMapboxTileCount;
    }

    // clang-format off
    mapbox::sqlite::Query query{ getStatement(
        "SELECT COUNT(DISTINCT id) "
        "FROM region_tiles, tiles "
        "WHERE tile_id = tiles.id "
        "AND url_template LIKE 'mapbox://%' ") };
    // clang-format on

    query.run();

    offlineMapboxTileCount = query.get<int64_t>(0);
    return *offlineMapboxTileCount;
} catch (const mapbox::sqlite::Exception& ex) {
    handleError(ex, "get offline Mapbox tile count");
    return std::numeric_limits<uint64_t>::max();
}

bool OfflineDatabase::exceedsOfflineMapboxTileCountLimit(const Resource& resource) {
    return resource.kind == Resource::Kind::Tile
        && util::mapbox::isMapboxURL(resource.url)
        && offlineMapboxTileCountLimitExceeded();
}

} // namespace mbgl
