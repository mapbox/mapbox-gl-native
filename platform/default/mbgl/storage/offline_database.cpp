#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/compression.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/logging.hpp>

#include "sqlite3.hpp"

namespace mbgl {

OfflineDatabase::Statement::~Statement() {
    stmt.reset();
    stmt.clearBindings();
}

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
        Log::Error(Event::Database, ex.code, ex.what());
    }
}

void OfflineDatabase::connect(int flags) {
    db = std::make_unique<mapbox::sqlite::Database>(path.c_str(), flags);
    db->setBusyTimeout(Milliseconds::max());
    db->exec("PRAGMA foreign_keys = ON");
}

void OfflineDatabase::ensureSchema() {
    if (path != ":memory:") {
        try {
            connect(mapbox::sqlite::ReadWrite);

            switch (userVersion()) {
            case 0: break; // cache-only database; ok to delete
            case 1: break; // cache-only database; ok to delete
            case 2: migrateToVersion3(); // fall through
            case 3: // no-op and fall through
            case 4: migrateToVersion5(); // fall through
            case 5: migrateToVersion6(); // fall through
            case 6: return;
            default: break; // downgrade, delete the database
            }

            removeExisting();
            connect(mapbox::sqlite::ReadWrite | mapbox::sqlite::Create);
        } catch (mapbox::sqlite::Exception& ex) {
            if (ex.code != mapbox::sqlite::Exception::Code::CANTOPEN && ex.code != mapbox::sqlite::Exception::Code::NOTADB) {
                Log::Error(Event::Database, "Unexpected error connecting to database: %s", ex.what());
                throw;
            }

            try {
                if (ex.code == mapbox::sqlite::Exception::Code::NOTADB) {
                    removeExisting();
                }
                connect(mapbox::sqlite::ReadWrite | mapbox::sqlite::Create);
            } catch (...) {
                Log::Error(Event::Database, "Unexpected error creating database: %s", util::toString(std::current_exception()).c_str());
                throw;
            }
        }
    }

    try {
        #include "offline_schema.cpp.include"

        connect(mapbox::sqlite::ReadWrite | mapbox::sqlite::Create);

        // If you change the schema you must write a migration from the previous version.
        db->exec("PRAGMA auto_vacuum = INCREMENTAL");
        db->exec("PRAGMA journal_mode = DELETE");
        db->exec("PRAGMA synchronous = FULL");
        db->exec(schema);
        db->exec("PRAGMA user_version = 6");
    } catch (...) {
        Log::Error(Event::Database, "Unexpected error creating database schema: %s", util::toString(std::current_exception()).c_str());
        throw;
    }
}

int OfflineDatabase::userVersion() {
    auto stmt = db->prepare("PRAGMA user_version");
    stmt.run();
    return stmt.get<int>(0);
}

void OfflineDatabase::removeExisting() {
    Log::Warning(Event::Database, "Removing existing incompatible offline database");

    db.reset();

    try {
        util::deleteFile(path);
    } catch (util::IOException& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }
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

OfflineDatabase::Statement OfflineDatabase::getStatement(const char * sql) {
    auto it = statements.find(sql);

    if (it != statements.end()) {
        return Statement(*it->second);
    }

    return Statement(*statements.emplace(sql, std::make_unique<mapbox::sqlite::Statement>(db->prepare(sql))).first->second);
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
    return putInternal(resource, response, true);
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
        Log::Debug(Event::Database, "Unable to make space for entry");
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
    // clang-format off
    Statement accessedStmt = getStatement(
        "UPDATE resources SET accessed = ?1 WHERE url = ?2");
    // clang-format on

    accessedStmt->bind(1, util::now());
    accessedStmt->bind(2, resource.url);
    accessedStmt->run();

    // clang-format off
    Statement stmt = getStatement(
        //        0      1            2            3       4      5
        "SELECT etag, expires, must_revalidate, modified, data, compressed "
        "FROM resources "
        "WHERE url = ?");
    // clang-format on

    stmt->bind(1, resource.url);

    if (!stmt->run()) {
        return {};
    }

    Response response;
    uint64_t size = 0;

    response.etag           = stmt->get<optional<std::string>>(0);
    response.expires        = stmt->get<optional<Timestamp>>(1);
    response.mustRevalidate = stmt->get<bool>(2);
    response.modified       = stmt->get<optional<Timestamp>>(3);

    optional<std::string> data = stmt->get<optional<std::string>>(4);
    if (!data) {
        response.noContent = true;
    } else if (stmt->get<bool>(5)) {
        response.data = std::make_shared<std::string>(util::decompress(*data));
        size = data->length();
    } else {
        response.data = std::make_shared<std::string>(*data);
        size = data->length();
    }

    return std::make_pair(response, size);
}

optional<int64_t> OfflineDatabase::hasResource(const Resource& resource) {
    // clang-format off
    Statement stmt = getStatement("SELECT length(data) FROM resources WHERE url = ?");
    // clang-format on

    stmt->bind(1, resource.url);
    if (!stmt->run()) {
        return {};
    }

    return stmt->get<optional<int64_t>>(0);
}

bool OfflineDatabase::putResource(const Resource& resource,
                                  const Response& response,
                                  const std::string& data,
                                  bool compressed) {
    if (response.notModified) {
        // clang-format off
        Statement update = getStatement(
            "UPDATE resources "
            "SET accessed         = ?1, "
            "    expires          = ?2, "
            "    must_revalidate  = ?3 "
            "WHERE url    = ?4 ");
        // clang-format on

        update->bind(1, util::now());
        update->bind(2, response.expires);
        update->bind(3, response.mustRevalidate);
        update->bind(4, resource.url);
        update->run();
        return false;
    }

    // We can't use REPLACE because it would change the id value.

    // Begin an immediate-mode transaction to ensure that two writers do not attempt
    // to INSERT a resource at the same moment.
    mapbox::sqlite::Transaction transaction(*db, mapbox::sqlite::Transaction::Immediate);

    // clang-format off
    Statement update = getStatement(
        "UPDATE resources "
        "SET kind            = ?1, "
        "    etag            = ?2, "
        "    expires         = ?3, "
        "    must_revalidate = ?4, "
        "    modified        = ?5, "
        "    accessed        = ?6, "
        "    data            = ?7, "
        "    compressed      = ?8 "
        "WHERE url           = ?9 ");
    // clang-format on

    update->bind(1, int(resource.kind));
    update->bind(2, response.etag);
    update->bind(3, response.expires);
    update->bind(4, response.mustRevalidate);
    update->bind(5, response.modified);
    update->bind(6, util::now());
    update->bind(9, resource.url);

    if (response.noContent) {
        update->bind(7, nullptr);
        update->bind(8, false);
    } else {
        update->bindBlob(7, data.data(), data.size(), false);
        update->bind(8, compressed);
    }

    update->run();
    if (update->changes() != 0) {
        transaction.commit();
        return false;
    }

    // clang-format off
    Statement insert = getStatement(
        "INSERT INTO resources (url, kind, etag, expires, must_revalidate, modified, accessed, data, compressed) "
        "VALUES                (?1,  ?2,   ?3,   ?4,      ?5,              ?6,       ?7,       ?8,   ?9) ");
    // clang-format on

    insert->bind(1, resource.url);
    insert->bind(2, int(resource.kind));
    insert->bind(3, response.etag);
    insert->bind(4, response.expires);
    insert->bind(5, response.mustRevalidate);
    insert->bind(6, response.modified);
    insert->bind(7, util::now());

    if (response.noContent) {
        insert->bind(8, nullptr);
        insert->bind(9, false);
    } else {
        insert->bindBlob(8, data.data(), data.size(), false);
        insert->bind(9, compressed);
    }

    insert->run();
    transaction.commit();

    return true;
}

optional<std::pair<Response, uint64_t>> OfflineDatabase::getTile(const Resource::TileData& tile) {
    // clang-format off
    Statement accessedStmt = getStatement(
        "UPDATE tiles "
        "SET accessed       = ?1 "
        "WHERE url_template = ?2 "
        "  AND pixel_ratio  = ?3 "
        "  AND x            = ?4 "
        "  AND y            = ?5 "
        "  AND z            = ?6 ");
    // clang-format on

    accessedStmt->bind(1, util::now());
    accessedStmt->bind(2, tile.urlTemplate);
    accessedStmt->bind(3, tile.pixelRatio);
    accessedStmt->bind(4, tile.x);
    accessedStmt->bind(5, tile.y);
    accessedStmt->bind(6, tile.z);
    accessedStmt->run();

    // clang-format off
    Statement stmt = getStatement(
        //        0      1           2,            3,      4,      5
        "SELECT etag, expires, must_revalidate, modified, data, compressed "
        "FROM tiles "
        "WHERE url_template = ?1 "
        "  AND pixel_ratio  = ?2 "
        "  AND x            = ?3 "
        "  AND y            = ?4 "
        "  AND z            = ?5 ");
    // clang-format on

    stmt->bind(1, tile.urlTemplate);
    stmt->bind(2, tile.pixelRatio);
    stmt->bind(3, tile.x);
    stmt->bind(4, tile.y);
    stmt->bind(5, tile.z);

    if (!stmt->run()) {
        return {};
    }

    Response response;
    uint64_t size = 0;

    response.etag            = stmt->get<optional<std::string>>(0);
    response.expires         = stmt->get<optional<Timestamp>>(1);
    response.mustRevalidate  = stmt->get<bool>(2);
    response.modified        = stmt->get<optional<Timestamp>>(3);

    optional<std::string> data = stmt->get<optional<std::string>>(4);
    if (!data) {
        response.noContent = true;
    } else if (stmt->get<bool>(5)) {
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
    Statement stmt = getStatement(
        "SELECT length(data) "
        "FROM tiles "
        "WHERE url_template = ?1 "
        "  AND pixel_ratio  = ?2 "
        "  AND x            = ?3 "
        "  AND y            = ?4 "
        "  AND z            = ?5 ");
    // clang-format on

    stmt->bind(1, tile.urlTemplate);
    stmt->bind(2, tile.pixelRatio);
    stmt->bind(3, tile.x);
    stmt->bind(4, tile.y);
    stmt->bind(5, tile.z);

    if (!stmt->run()) {
        return {};
    }

    return stmt->get<optional<int64_t>>(0);
}

bool OfflineDatabase::putTile(const Resource::TileData& tile,
                              const Response& response,
                              const std::string& data,
                              bool compressed) {
    if (response.notModified) {
        // clang-format off
        Statement update = getStatement(
            "UPDATE tiles "
            "SET accessed        = ?1, "
            "    expires         = ?2, "
            "    must_revalidate = ?3 "
            "WHERE url_template  = ?4 "
            "  AND pixel_ratio   = ?5 "
            "  AND x             = ?6 "
            "  AND y             = ?7 "
            "  AND z             = ?8 ");
        // clang-format on

        update->bind(1, util::now());
        update->bind(2, response.expires);
        update->bind(3, response.mustRevalidate);
        update->bind(4, tile.urlTemplate);
        update->bind(5, tile.pixelRatio);
        update->bind(6, tile.x);
        update->bind(7, tile.y);
        update->bind(8, tile.z);
        update->run();
        return false;
    }

    // We can't use REPLACE because it would change the id value.

    // Begin an immediate-mode transaction to ensure that two writers do not attempt
    // to INSERT a resource at the same moment.
    mapbox::sqlite::Transaction transaction(*db, mapbox::sqlite::Transaction::Immediate);

    // clang-format off
    Statement update = getStatement(
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
        "  AND z             = ?12 ");
    // clang-format on

    update->bind(1, response.modified);
    update->bind(2, response.etag);
    update->bind(3, response.expires);
    update->bind(4, response.mustRevalidate);
    update->bind(5, util::now());
    update->bind(8, tile.urlTemplate);
    update->bind(9, tile.pixelRatio);
    update->bind(10, tile.x);
    update->bind(11, tile.y);
    update->bind(12, tile.z);

    if (response.noContent) {
        update->bind(6, nullptr);
        update->bind(7, false);
    } else {
        update->bindBlob(6, data.data(), data.size(), false);
        update->bind(7, compressed);
    }

    update->run();
    if (update->changes() != 0) {
        transaction.commit();
        return false;
    }

    // clang-format off
    Statement insert = getStatement(
        "INSERT INTO tiles (url_template, pixel_ratio, x,  y,  z,  modified, must_revalidate, etag, expires, accessed,  data, compressed) "
        "VALUES            (?1,           ?2,          ?3, ?4, ?5, ?6,       ?7,              ?8,   ?9,      ?10,       ?11,  ?12)");
    // clang-format on

    insert->bind(1, tile.urlTemplate);
    insert->bind(2, tile.pixelRatio);
    insert->bind(3, tile.x);
    insert->bind(4, tile.y);
    insert->bind(5, tile.z);
    insert->bind(6, response.modified);
    insert->bind(7, response.mustRevalidate);
    insert->bind(8, response.etag);
    insert->bind(9, response.expires);
    insert->bind(10, util::now());

    if (response.noContent) {
        insert->bind(11, nullptr);
        insert->bind(12, false);
    } else {
        insert->bindBlob(11, data.data(), data.size(), false);
        insert->bind(12, compressed);
    }

    insert->run();
    transaction.commit();

    return true;
}

std::vector<OfflineRegion> OfflineDatabase::listRegions() {
    // clang-format off
    Statement stmt = getStatement(
        "SELECT id, definition, description FROM regions");
    // clang-format on

    std::vector<OfflineRegion> result;

    while (stmt->run()) {
        result.push_back(OfflineRegion(
            stmt->get<int64_t>(0),
            decodeOfflineRegionDefinition(stmt->get<std::string>(1)),
            stmt->get<std::vector<uint8_t>>(2)));
    }

    return result;
}

OfflineRegion OfflineDatabase::createRegion(const OfflineRegionDefinition& definition,
                                            const OfflineRegionMetadata& metadata) {
    // clang-format off
    Statement stmt = getStatement(
        "INSERT INTO regions (definition, description) "
        "VALUES              (?1,         ?2) ");
    // clang-format on

    stmt->bind(1, encodeOfflineRegionDefinition(definition));
    stmt->bindBlob(2, metadata);
    stmt->run();

    return OfflineRegion(stmt->lastInsertRowId(), definition, metadata);
}

OfflineRegionMetadata OfflineDatabase::updateMetadata(const int64_t regionID, const OfflineRegionMetadata& metadata) {
    // clang-format off
    Statement stmt = getStatement(
                                  "UPDATE regions SET description = ?1"
                                  "WHERE id = ?2");
    // clang-format on
    stmt->bindBlob(1, metadata);
    stmt->bind(2, regionID);
    stmt->run();

    return metadata;
}

void OfflineDatabase::deleteRegion(OfflineRegion&& region) {
    // clang-format off
    Statement stmt = getStatement(
        "DELETE FROM regions WHERE id = ?");
    // clang-format on

    stmt->bind(1, region.getID());
    stmt->run();

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
        Statement insert = getStatement(
            "INSERT OR IGNORE INTO region_tiles (region_id, tile_id) "
            "SELECT                              ?1,        tiles.id "
            "FROM tiles "
            "WHERE url_template = ?2 "
            "  AND pixel_ratio  = ?3 "
            "  AND x            = ?4 "
            "  AND y            = ?5 "
            "  AND z            = ?6 ");
        // clang-format on

        const Resource::TileData& tile = *resource.tileData;
        insert->bind(1, regionID);
        insert->bind(2, tile.urlTemplate);
        insert->bind(3, tile.pixelRatio);
        insert->bind(4, tile.x);
        insert->bind(5, tile.y);
        insert->bind(6, tile.z);
        insert->run();

        if (insert->changes() == 0) {
            return false;
        }

        // clang-format off
        Statement select = getStatement(
            "SELECT region_id "
            "FROM region_tiles, tiles "
            "WHERE region_id   != ?1 "
            "  AND url_template = ?2 "
            "  AND pixel_ratio  = ?3 "
            "  AND x            = ?4 "
            "  AND y            = ?5 "
            "  AND z            = ?6 "
            "LIMIT 1 ");
        // clang-format on

        select->bind(1, regionID);
        select->bind(2, tile.urlTemplate);
        select->bind(3, tile.pixelRatio);
        select->bind(4, tile.x);
        select->bind(5, tile.y);
        select->bind(6, tile.z);
        return !select->run();
    } else {
        // clang-format off
        Statement insert = getStatement(
            "INSERT OR IGNORE INTO region_resources (region_id, resource_id) "
            "SELECT                                  ?1,        resources.id "
            "FROM resources "
            "WHERE resources.url = ?2 ");
        // clang-format on

        insert->bind(1, regionID);
        insert->bind(2, resource.url);
        insert->run();

        if (insert->changes() == 0) {
            return false;
        }

        // clang-format off
        Statement select = getStatement(
            "SELECT region_id "
            "FROM region_resources, resources "
            "WHERE region_id    != ?1 "
            "  AND resources.url = ?2 "
            "LIMIT 1 ");
        // clang-format on

        select->bind(1, regionID);
        select->bind(2, resource.url);
        return !select->run();
    }
}

OfflineRegionDefinition OfflineDatabase::getRegionDefinition(int64_t regionID) {
    // clang-format off
    Statement stmt = getStatement(
        "SELECT definition FROM regions WHERE id = ?1");
    // clang-format on

    stmt->bind(1, regionID);
    stmt->run();

    return decodeOfflineRegionDefinition(stmt->get<std::string>(0));
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
    Statement stmt = getStatement(
        "SELECT COUNT(*), SUM(LENGTH(data)) "
        "FROM region_resources, resources "
        "WHERE region_id = ?1 "
        "AND resource_id = resources.id ");
    // clang-format on
    stmt->bind(1, regionID);
    stmt->run();
    return { stmt->get<int64_t>(0), stmt->get<int64_t>(1) };
}

std::pair<int64_t, int64_t> OfflineDatabase::getCompletedTileCountAndSize(int64_t regionID) {
    // clang-format off
    Statement stmt = getStatement(
        "SELECT COUNT(*), SUM(LENGTH(data)) "
        "FROM region_tiles, tiles "
        "WHERE region_id = ?1 "
        "AND tile_id = tiles.id ");
    // clang-format on
    stmt->bind(1, regionID);
    stmt->run();
    return { stmt->get<int64_t>(0), stmt->get<int64_t>(1) };
}

template <class T>
T OfflineDatabase::getPragma(const char * sql) {
    Statement stmt = getStatement(sql);
    stmt->run();
    return stmt->get<T>(0);
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
        Statement accessedStmt = getStatement(
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
        );
        accessedStmt->bind(1, 50);
        // clang-format on
        if (!accessedStmt->run()) {
            return false;
        }
        Timestamp accessed = accessedStmt->get<Timestamp>(0);

        // clang-format off
        Statement stmt1 = getStatement(
            "DELETE FROM resources "
            "WHERE id IN ( "
            "  SELECT id FROM resources "
            "  LEFT JOIN region_resources "
            "  ON resource_id = resources.id "
            "  WHERE resource_id IS NULL "
            "  AND accessed <= ?1 "
            ") ");
        // clang-format on
        stmt1->bind(1, accessed);
        stmt1->run();
        uint64_t changes1 = stmt1->changes();

        // clang-format off
        Statement stmt2 = getStatement(
            "DELETE FROM tiles "
            "WHERE id IN ( "
            "  SELECT id FROM tiles "
            "  LEFT JOIN region_tiles "
            "  ON tile_id = tiles.id "
            "  WHERE tile_id IS NULL "
            "  AND accessed <= ?1 "
            ") ");
        // clang-format on
        stmt2->bind(1, accessed);
        stmt2->run();
        uint64_t changes2 = stmt2->changes();

        // The cached value of offlineTileCount does not need to be updated
        // here because only non-offline tiles can be removed by eviction.

        if (changes1 == 0 && changes2 == 0) {
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
    Statement stmt = getStatement(
        "SELECT COUNT(DISTINCT id) "
        "FROM region_tiles, tiles "
        "WHERE tile_id = tiles.id "
        "AND url_template LIKE 'mapbox://%' ");
    // clang-format on

    stmt->run();

    offlineMapboxTileCount = stmt->get<int64_t>(0);
    return *offlineMapboxTileCount;
}

} // namespace mbgl
