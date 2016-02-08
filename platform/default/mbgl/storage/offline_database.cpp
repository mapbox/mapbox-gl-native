#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/compression.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/platform/log.hpp>

#include "sqlite3.hpp"
#include <sqlite3.h>

namespace mbgl {

using namespace mapbox::sqlite;

// If you change the schema you must write a migration from the previous version.
static const uint32_t schemaVersion = 2;

OfflineDatabase::OfflineDatabase(const std::string& path_)
    : path(path_) {
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

void OfflineDatabase::ensureSchema() {
    if (path != ":memory:") {
        try {
            db = std::make_unique<Database>(path.c_str(), ReadWrite);
            db->setBusyTimeout(Milliseconds::max());

            {
                Statement userVersionStmt(db->prepare("PRAGMA user_version"));
                userVersionStmt.run();
                switch (userVersionStmt.get<int>(0)) {
                case 0: break; // cache-only database; ok to delete
                case 1: break; // cache-only database; ok to delete
                case 2: return;
                default: throw std::runtime_error("unknown schema version");
                }
            }

            removeExisting();
            db = std::make_unique<Database>(path.c_str(), ReadWrite | Create);
            db->setBusyTimeout(Milliseconds::max());
        } catch (mapbox::sqlite::Exception& ex) {
            if (ex.code == SQLITE_CANTOPEN) {
                db = std::make_unique<Database>(path.c_str(), ReadWrite | Create);
                db->setBusyTimeout(Milliseconds::max());
            } else if (ex.code == SQLITE_NOTADB) {
                removeExisting();
                db = std::make_unique<Database>(path.c_str(), ReadWrite | Create);
                db->setBusyTimeout(Milliseconds::max());
            }
        }
    }

    #include "offline_schema.cpp.include"

    db = std::make_unique<Database>(path.c_str(), ReadWrite | Create);
    db->setBusyTimeout(Milliseconds::max());
    db->exec(schema);
    db->exec("PRAGMA user_version = " + util::toString(schemaVersion));
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

mapbox::sqlite::Statement& OfflineDatabase::getStatement(const char * sql) {
    auto it = statements.find(sql);

    if (it != statements.end()) {
        it->second->reset();
        return *it->second;
    }

    return *(statements[sql] = std::make_unique<Statement>(db->prepare(sql)));
}

optional<Response> OfflineDatabase::get(const Resource& resource) {
    if (resource.kind == Resource::Kind::Tile) {
        assert(resource.tileData);
        return getTile(*resource.tileData);
    } else {
        return getResource(resource);
    }
}

uint64_t OfflineDatabase::put(const Resource& resource, const Response& response) {
    // Don't store errors in the cache.
    if (response.error) {
        return 0;
    }

    if (resource.kind == Resource::Kind::Tile) {
        assert(resource.tileData);
        return putTile(*resource.tileData, response);
    } else {
        return putResource(resource, response);
    }
}

optional<Response> OfflineDatabase::getResource(const Resource& resource) {
    mapbox::sqlite::Statement& stmt = getStatement(
        //        0      1        2       3        4
        "SELECT etag, expires, modified, data, compressed "
        "FROM resources "
        "WHERE url = ?");

    stmt.bind(1, resource.url);

    if (!stmt.run()) {
        return {};
    }

    Response response;

    response.etag     = stmt.get<optional<std::string>>(0);
    response.expires  = stmt.get<optional<SystemTimePoint>>(1);
    response.modified = stmt.get<optional<SystemTimePoint>>(2);

    optional<std::string> data = stmt.get<optional<std::string>>(3);
    if (!data) {
        response.noContent = true;
    } else if (stmt.get<int>(4)) {
        response.data = std::make_shared<std::string>(util::decompress(*data));
    } else {
        response.data = std::make_shared<std::string>(*data);
    }

    return response;
}

uint64_t OfflineDatabase::putResource(const Resource& resource, const Response& response) {
    if (response.notModified) {
        mapbox::sqlite::Statement& stmt = getStatement(
            //                               1            2             3
            "UPDATE resources SET accessed = ?, expires = ? WHERE url = ?");

        stmt.bind(1, SystemClock::now());
        stmt.bind(2, response.expires);
        stmt.bind(3, resource.url);
        stmt.run();
        return 0;
    } else {
        mapbox::sqlite::Statement& stmt = getStatement(
            //                        1     2    3      4        5         6        7       8
            "REPLACE INTO resources (url, kind, etag, expires, modified, accessed, data, compressed) "
            "VALUES                 (?,   ?,    ?,    ?,       ?,        ?,        ?,    ?)");

        stmt.bind(1 /* url */, resource.url);
        stmt.bind(2 /* kind */, int(resource.kind));
        stmt.bind(3 /* etag */, response.etag);
        stmt.bind(4 /* expires */, response.expires);
        stmt.bind(5 /* modified */, response.modified);
        stmt.bind(6 /* accessed */, SystemClock::now());

        std::string data;
        uint64_t size = 0;

        if (response.noContent) {
            stmt.bind(7 /* data */, nullptr);
            stmt.bind(8 /* compressed */, false);
        } else {
            data = util::compress(*response.data);
            if (data.size() < response.data->size()) {
                size = data.size();
                stmt.bindBlob(7 /* data */, data.data(), size, false);
                stmt.bind(8 /* compressed */, true);
            } else {
                size = response.data->size();
                stmt.bindBlob(7 /* data */, response.data->data(), size, false);
                stmt.bind(8 /* compressed */, false);
            }
        }

        stmt.run();
        return size;
    }
}

optional<Response> OfflineDatabase::getTile(const Resource::TileData& tile) {
    mapbox::sqlite::Statement& stmt = getStatement(
        //        0      1        2       3        4
        "SELECT etag, expires, modified, data, compressed "
        "FROM tilesets, tiles "
        "WHERE tilesets.url_template = ? " // 1
        "AND tilesets.pixel_ratio = ? "    // 2
        "AND tiles.x = ? "                 // 3
        "AND tiles.y = ? "                 // 4
        "AND tiles.z = ? "                 // 5
        "AND tilesets.id = tiles.tileset_id ");

    stmt.bind(1, tile.urlTemplate);
    stmt.bind(2, tile.pixelRatio);
    stmt.bind(3, tile.x);
    stmt.bind(4, tile.y);
    stmt.bind(5, tile.z);

    if (!stmt.run()) {
        return {};
    }

    Response response;

    response.etag     = stmt.get<optional<std::string>>(0);
    response.expires  = stmt.get<optional<SystemTimePoint>>(1);
    response.modified = stmt.get<optional<SystemTimePoint>>(2);

    optional<std::string> data = stmt.get<optional<std::string>>(3);
    if (!data) {
        response.noContent = true;
    } else if (stmt.get<int>(4)) {
        response.data = std::make_shared<std::string>(util::decompress(*data));
    } else {
        response.data = std::make_shared<std::string>(*data);
    }

    return response;
}

uint64_t OfflineDatabase::putTile(const Resource::TileData& tile, const Response& response) {
    if (response.notModified) {
        mapbox::sqlite::Statement& stmt = getStatement(
            "UPDATE tiles SET accessed = ?1, expires = ?2 "
            "WHERE tileset_id = ( "
            "  SELECT id FROM tilesets "
            "  WHERE url_template = ?3 "
            "  AND pixel_ratio = ?4) "
            "AND tiles.x = ?5 "
            "AND tiles.y = ?6 "
            "AND tiles.z = ?7 ");

        stmt.bind(1, SystemClock::now());
        stmt.bind(2, response.expires);
        stmt.bind(3, tile.urlTemplate);
        stmt.bind(4, tile.pixelRatio);
        stmt.bind(5, tile.x);
        stmt.bind(6, tile.y);
        stmt.bind(7, tile.z);
        stmt.run();
        return 0;
    } else {
        mapbox::sqlite::Statement& stmt1 = getStatement(
            "REPLACE INTO tilesets (url_template, pixel_ratio) "
            "VALUES                (?1,           ?2) ");

        stmt1.bind(1 /* url_template */, tile.urlTemplate);
        stmt1.bind(2 /* pixel_ratio */, tile.pixelRatio);
        stmt1.run();

        mapbox::sqlite::Statement& stmt2 = getStatement(
            "REPLACE INTO tiles (tileset_id,  x,  y,  z,  modified,  etag,  expires,  accessed,  data, compressed) "
            "SELECT              tilesets.id, ?3, ?4, ?5, ?6,        ?7,    ?8,       ?9,        ?10,  ?11 "
            "FROM tilesets "
            "WHERE url_template = ?1 "
            "AND pixel_ratio    = ?2 ");

        stmt2.bind(1 /* url_template */, tile.urlTemplate);
        stmt2.bind(2 /* pixel_ratio */, tile.pixelRatio);
        stmt2.bind(3 /* x */, tile.x);
        stmt2.bind(4 /* y */, tile.y);
        stmt2.bind(5 /* z */, tile.z);
        stmt2.bind(6 /* modified */, response.modified);
        stmt2.bind(7 /* etag */, response.etag);
        stmt2.bind(8 /* expires */, response.expires);
        stmt2.bind(9 /* accessed */, SystemClock::now());

        std::string data;
        uint64_t size = 0;

        if (response.noContent) {
            stmt2.bind(10 /* data */, nullptr);
            stmt2.bind(11 /* compressed */, false);
        } else {
            data = util::compress(*response.data);
            if (data.size() < response.data->size()) {
                size = data.size();
                stmt2.bindBlob(10 /* data */, data.data(), size, false);
                stmt2.bind(11 /* compressed */, true);
            } else {
                size = response.data->size();
                stmt2.bindBlob(10 /* data */, response.data->data(), size, false);
                stmt2.bind(11 /* compressed */, false);
            }
        }

        stmt2.run();
        return size;
    }
}

std::vector<OfflineRegion> OfflineDatabase::listRegions() {
    mapbox::sqlite::Statement& stmt = getStatement(
        "SELECT id, definition, description FROM regions");

    std::vector<OfflineRegion> result;

    while (stmt.run()) {
        result.push_back(OfflineRegion(
            stmt.get<int64_t>(0),
            decodeOfflineRegionDefinition(stmt.get<std::string>(1)),
            stmt.get<std::vector<uint8_t>>(2)));
    }

    return std::move(result);
}

OfflineRegion OfflineDatabase::createRegion(const OfflineRegionDefinition& definition,
                                            const OfflineRegionMetadata& metadata) {
    mapbox::sqlite::Statement& stmt = getStatement(
        "INSERT INTO regions (definition, description) "
        "VALUES              (?1,         ?2) ");

    stmt.bind(1, encodeOfflineRegionDefinition(definition));
    stmt.bindBlob(2, metadata);
    stmt.run();

    return OfflineRegion(db->lastInsertRowid(), definition, metadata);
}

void OfflineDatabase::deleteRegion(OfflineRegion&& region) {
    mapbox::sqlite::Statement& stmt = getStatement(
        "DELETE FROM regions WHERE id = ?");

    stmt.bind(1, region.getID());
    stmt.run();
}

optional<Response> OfflineDatabase::getRegionResource(int64_t regionID, const Resource& resource) {
    auto response = get(resource);

    if (response) {
        markUsed(regionID, resource);
    }

    return response;
}

uint64_t OfflineDatabase::putRegionResource(int64_t regionID, const Resource& resource, const Response& response) {
    uint64_t result = put(resource, response);
    markUsed(regionID, resource);
    return result;
}

void OfflineDatabase::markUsed(int64_t regionID, const Resource& resource) {
    if (resource.kind == Resource::Kind::Tile) {
        mapbox::sqlite::Statement& stmt1 = getStatement(
            "REPLACE INTO region_tiles (region_id, tileset_id,  x,  y,  z) "
            "SELECT                     ?1,        tilesets.id, ?4, ?5, ?6 "
            "FROM tilesets "
            "WHERE url_template = ?2 "
            "AND pixel_ratio    = ?3 ");

        stmt1.bind(1, regionID);
        stmt1.bind(2, (*resource.tileData).urlTemplate);
        stmt1.bind(3, (*resource.tileData).pixelRatio);
        stmt1.bind(4, (*resource.tileData).x);
        stmt1.bind(5, (*resource.tileData).y);
        stmt1.bind(6, (*resource.tileData).z);
        stmt1.run();
    } else {
        mapbox::sqlite::Statement& stmt1 = getStatement(
            "REPLACE INTO region_resources (region_id, resource_url) "
            "VALUES                        (?1,        ?2) ");

        stmt1.bind(1, regionID);
        stmt1.bind(2, resource.url);
        stmt1.run();
    }
}

OfflineRegionDefinition OfflineDatabase::getRegionDefinition(int64_t regionID) {
    mapbox::sqlite::Statement& stmt = getStatement(
        "SELECT definition FROM regions WHERE id = ?1");

    stmt.bind(1, regionID);
    stmt.run();

    return decodeOfflineRegionDefinition(stmt.get<std::string>(0));
}

OfflineRegionStatus OfflineDatabase::getRegionCompletedStatus(int64_t regionID) {
    OfflineRegionStatus result;

    mapbox::sqlite::Statement& stmt = getStatement(
        "SELECT COUNT(*), SUM(size) FROM ( "
        "  SELECT LENGTH(data) as size "
        "  FROM region_resources, resources "
        "  WHERE region_id = ?1 "
        "  AND resources.url = region_resources.resource_url "
        "  UNION ALL "
        "  SELECT LENGTH(data) as size "
        "  FROM region_tiles, tiles "
        "  WHERE region_id = ?1 "
        "  AND tiles.tileset_id = region_tiles.tileset_id "
        "  AND tiles.z = region_tiles.z "
        "  AND tiles.x = region_tiles.x "
        "  AND tiles.y = region_tiles.y "
        ") ");

    stmt.bind(1, regionID);
    stmt.run();

    result.completedResourceCount = stmt.get<int64_t>(0);
    result.completedResourceSize  = stmt.get<int64_t>(1);

    return result;
}

void OfflineDatabase::removeUnusedResources() {
    mapbox::sqlite::Statement& stmt1 = getStatement(
        "DELETE FROM resources "
        "WHERE ROWID NOT IN ( "
        "  SELECT resources.ROWID "
        "  FROM resources, region_resources "
        "  WHERE resources.url = region_resources.resource_url "
        ") ");
    stmt1.run();

    mapbox::sqlite::Statement& stmt2 = getStatement(
        "DELETE FROM tiles "
        "WHERE ROWID NOT IN ( "
        "  SELECT tiles.ROWID "
        "  FROM tiles, region_tiles "
        "  AND tiles.tileset_id = region_tiles.tileset_id "
        "  AND tiles.z = region_tiles.z "
        "  AND tiles.x = region_tiles.x "
        "  AND tiles.y = region_tiles.y "
        ") ");
    stmt2.run();
}

} // namespace mbgl
