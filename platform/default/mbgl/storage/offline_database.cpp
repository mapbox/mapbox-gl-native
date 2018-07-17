#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/compression.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/logging.hpp>

#include "offline_schema.hpp"

#include "sqlite3.hpp"

namespace mbgl {

OfflineDatabase::OfflineDatabase(std::string path_, uint64_t maximumCacheSize_)
    : path(std::move(path_)),
      maximumCacheSize(maximumCacheSize_) {
    ensureSchema();
}

OfflineDatabase::~OfflineDatabase() {
    // Deleting these SQLite objects may result in exceptions, but we're in a destructor, so we
    // can't throw anything.
    try {
        statements.clear();
        db.reset();
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, (int)ex.code, ex.what());
    }
}

void OfflineDatabase::ensureSchema() {
    auto result = mapbox::sqlite::Database::tryOpen(path, mapbox::sqlite::ReadWriteCreate);
    if (result.is<mapbox::sqlite::Exception>()) {
        const auto& ex = result.get<mapbox::sqlite::Exception>();
        if (ex.code == mapbox::sqlite::ResultCode::NotADB) {
            // Corrupted; blow it away.
            removeExisting();
            result = mapbox::sqlite::Database::open(path, mapbox::sqlite::ReadWriteCreate);
        } else {
            Log::Error(Event::Database, "Unexpected error connecting to database: %s", ex.what());
            throw ex;
        }
    }

    try {
        assert(result.is<mapbox::sqlite::Database>());
        db = std::make_unique<mapbox::sqlite::Database>(std::move(result.get<mapbox::sqlite::Database>()));
        db->setBusyTimeout(Milliseconds::max());
        db->exec("PRAGMA foreign_keys = ON");

        switch (userVersion()) {
        case 0:
        case 1:
            // Newly created database, or old cache-only database; remove old table if it exists.
            removeOldCacheTable();
            break;
        case 2:
            migrateToVersion3();
            // fall through
        case 3:
        case 4:
            migrateToVersion5();
            // fall through
        case 5:
            migrateToVersion6();
            // fall through
        case 6:
            // happy path; we're done
            return;
        default:
            // downgrade, delete the database
            removeExisting();
            break;
        }
    } catch (const mapbox::sqlite::Exception& ex) {
        // Unfortunately, SQLITE_NOTADB is not always reported upon opening the database.
        // Apparently sometimes it is delayed until the first read operation.
        if (ex.code == mapbox::sqlite::ResultCode::NotADB) {
            removeExisting();
        } else {
            throw;
        }
    }

    try {
        // When downgrading the database, or when the database is corrupt, we've deleted the old database handle,
        // so we need to reopen it.
        if (!db) {
            db = std::make_unique<mapbox::sqlite::Database>(mapbox::sqlite::Database::open(path, mapbox::sqlite::ReadWriteCreate));
            db->setBusyTimeout(Milliseconds::max());
            db->exec("PRAGMA foreign_keys = ON");
        }

        db->exec("PRAGMA auto_vacuum = INCREMENTAL");
        db->exec("PRAGMA journal_mode = DELETE");
        db->exec("PRAGMA synchronous = FULL");
        db->exec(offlineDatabaseSchema);
        db->exec("PRAGMA user_version = 6");
    } catch (...) {
        Log::Error(Event::Database, "Unexpected error creating database schema: %s", util::toString(std::current_exception()).c_str());
        throw;
    }
}

int OfflineDatabase::userVersion() {
    return static_cast<int>(getPragma<int64_t>("PRAGMA user_version"));
}

void OfflineDatabase::removeExisting() {
    Log::Warning(Event::Database, "Removing existing incompatible offline database");

    statements.clear();
    db.reset();

    try {
        util::deleteFile(path);
    } catch (util::IOException& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }
}

void OfflineDatabase::removeOldCacheTable() {
    db->exec("DROP TABLE IF EXISTS http_cache");
    db->exec("VACUUM");
}

void OfflineDatabase::migrateToVersion3() {
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
    db->exec("PRAGMA journal_mode = DELETE");
    db->exec("PRAGMA synchronous = FULL");
    db->exec("PRAGMA user_version = 5");
}

void OfflineDatabase::migrateToVersion6() {
    mapbox::sqlite::Transaction transaction(*db);
    db->exec("ALTER TABLE resources ADD COLUMN must_revalidate INTEGER NOT NULL DEFAULT 0");
    db->exec("ALTER TABLE tiles ADD COLUMN must_revalidate INTEGER NOT NULL DEFAULT 0");
    db->exec("PRAGMA user_version = 6");
    transaction.commit();
}

mapbox::sqlite::Statement& OfflineDatabase::getStatement(const char* sql) {
    auto it = statements.find(sql);
    if (it == statements.end()) {
        it = statements.emplace(sql, std::make_unique<mapbox::sqlite::Statement>(*db, sql)).first;
    }
    return *it->second;
}

optional<Response> OfflineDatabase::get(const Resource& resource) {
    auto result = getInternal(resource);
    return result ? result->first : optional<Response>();
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

std::pair<bool, uint64_t> OfflineDatabase::put(const Resource& resource, const Response& response) {
    mapbox::sqlite::Transaction transaction(*db, mapbox::sqlite::Transaction::Immediate);
    auto result = putInternal(resource, response, true);
    transaction.commit();
    return result;
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
    {
        mapbox::sqlite::Query accessedQuery{ getStatement("UPDATE resources SET accessed = ?1 WHERE url = ?2") };
        accessedQuery.bind(1, util::now());
        accessedQuery.bind(2, resource.url);
        accessedQuery.run();
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
        return {};
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
        return {};
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
    {
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
        return {};
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
        return {};
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

std::vector<OfflineRegion> OfflineDatabase::listRegions() {
    mapbox::sqlite::Query query{ getStatement("SELECT id, definition, description FROM regions") };

    std::vector<OfflineRegion> result;

    while (query.run()) {
        result.push_back(OfflineRegion(
            query.get<int64_t>(0),
            decodeOfflineRegionDefinition(query.get<std::string>(1)),
            query.get<std::vector<uint8_t>>(2)));
    }

    return result;
}

OfflineRegion OfflineDatabase::createRegion(const OfflineRegionDefinition& definition,
                                            const OfflineRegionMetadata& metadata) {
    // clang-format off
    mapbox::sqlite::Query query{ getStatement(
        "INSERT INTO regions (definition, description) "
        "VALUES              (?1,         ?2) ") };
    // clang-format on

    query.bind(1, encodeOfflineRegionDefinition(definition));
    query.bindBlob(2, metadata);
    query.run();

    return OfflineRegion(query.lastInsertRowId(), definition, metadata);
}

OfflineRegionMetadata OfflineDatabase::updateMetadata(const int64_t regionID, const OfflineRegionMetadata& metadata) {
    // clang-format off
    mapbox::sqlite::Query query{ getStatement(
                                  "UPDATE regions SET description = ?1 "
                                  "WHERE id = ?2") };
    // clang-format on
    query.bindBlob(1, metadata);
    query.bind(2, regionID);
    query.run();

    return metadata;
}

void OfflineDatabase::deleteRegion(OfflineRegion&& region) {
    {
        mapbox::sqlite::Query query{ getStatement("DELETE FROM regions WHERE id = ?") };
        query.bind(1, region.getID());
        query.run();
    }

    evict(0);
    db->exec("PRAGMA incremental_vacuum");

    // Ensure that the cached offlineTileCount value is recalculated.
    offlineMapboxTileCount = {};
}

optional<std::pair<Response, uint64_t>> OfflineDatabase::getRegionResource(int64_t regionID, const Resource& resource) {
    auto response = getInternal(resource);

    if (response) {
        markUsed(regionID, resource);
    }

    return response;
}

optional<int64_t> OfflineDatabase::hasRegionResource(int64_t regionID, const Resource& resource) {
    auto response = hasInternal(resource);

    if (response) {
        markUsed(regionID, resource);
    }

    return response;
}

uint64_t OfflineDatabase::putRegionResource(int64_t regionID, const Resource& resource, const Response& response) {
    mapbox::sqlite::Transaction transaction(*db);
    auto size = putRegionResourceInternal(regionID, resource, response);
    transaction.commit();
    return size;
}

void OfflineDatabase::putRegionResources(int64_t regionID, const std::list<std::tuple<Resource, Response>>& resources, OfflineRegionStatus& status) {
    mapbox::sqlite::Transaction transaction(*db);

    for (const auto& elem : resources) {
        const auto& resource = std::get<0>(elem);
        const auto& response = std::get<1>(elem);

        try {
            uint64_t resourceSize = putRegionResourceInternal(regionID, resource, response);
            status.completedResourceCount++;
            status.completedResourceSize += resourceSize;
            if (resource.kind == Resource::Kind::Tile) {
                status.completedTileCount += 1;
                status.completedTileSize += resourceSize;
            }
        } catch (const MapboxTileLimitExceededException&) {
            // Commit the rest of the batch and retrow
            transaction.commit();
            throw;
        }
    }

    // Commit the completed batch
    transaction.commit();
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
            "WHERE region_id    != ?1 "
            "  AND resources.url = ?2 "
            "LIMIT 1 ") };
        // clang-format on

        selectQuery.bind(1, regionID);
        selectQuery.bind(2, resource.url);
        return !selectQuery.run();
    }
}

OfflineRegionDefinition OfflineDatabase::getRegionDefinition(int64_t regionID) {
    mapbox::sqlite::Query query{ getStatement("SELECT definition FROM regions WHERE id = ?1") };
    query.bind(1, regionID);
    query.run();

    return decodeOfflineRegionDefinition(query.get<std::string>(0));
}

OfflineRegionStatus OfflineDatabase::getRegionCompletedStatus(int64_t regionID) {
    OfflineRegionStatus result;

    std::tie(result.completedResourceCount, result.completedResourceSize)
        = getCompletedResourceCountAndSize(regionID);
    std::tie(result.completedTileCount, result.completedTileSize)
        = getCompletedTileCountAndSize(regionID);

    result.completedResourceCount += result.completedTileCount;
    result.completedResourceSize += result.completedTileSize;

    return result;
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

uint64_t OfflineDatabase::getOfflineMapboxTileCount() {
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
}

bool OfflineDatabase::exceedsOfflineMapboxTileCountLimit(const Resource& resource) {
    return resource.kind == Resource::Kind::Tile
        && util::mapbox::isMapboxURL(resource.url)
        && offlineMapboxTileCountLimitExceeded();
}

} // namespace mbgl
