#pragma once

#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/offline.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/expected.hpp>

#include <unordered_map>
#include <memory>
#include <string>
#include <list>

namespace mapbox {
namespace sqlite {
class Database;
class Statement;
class Query;
class Exception;
} // namespace sqlite
} // namespace mapbox

namespace mbgl {

class Response;
class TileID;

namespace util {
struct IOException;
} // namespace util

struct MapboxTileLimitExceededException :  util::Exception {
    MapboxTileLimitExceededException() : util::Exception("Mapbox tile limit exceeded") {}
};

class OfflineDatabase : private util::noncopyable {
public:
    // Limits affect ambient caching (put) only; resources required by offline
    // regions are exempt.
    OfflineDatabase(std::string path, uint64_t maximumCacheSize = util::DEFAULT_MAX_CACHE_SIZE);
    ~OfflineDatabase();

    optional<Response> get(const Resource&);

    // Return value is (inserted, stored size)
    std::pair<bool, uint64_t> put(const Resource&, const Response&);

    expected<OfflineRegions, std::exception_ptr> listRegions();

    expected<OfflineRegion, std::exception_ptr> createRegion(const OfflineRegionDefinition&,
                                                             const OfflineRegionMetadata&);

    expected<OfflineRegions, std::exception_ptr>
    mergeDatabase(const std::string& sideDatabasePath);

    expected<OfflineRegionMetadata, std::exception_ptr>
    updateMetadata(const int64_t regionID, const OfflineRegionMetadata&);

    std::exception_ptr deleteRegion(OfflineRegion&&);

    // Return value is (response, stored size)
    optional<std::pair<Response, uint64_t>> getRegionResource(int64_t regionID, const Resource&);
    optional<int64_t> hasRegionResource(int64_t regionID, const Resource&);
    uint64_t putRegionResource(int64_t regionID, const Resource&, const Response&);
    void putRegionResources(int64_t regionID, const std::list<std::tuple<Resource, Response>>&, OfflineRegionStatus&);

    expected<OfflineRegionDefinition, std::exception_ptr> getRegionDefinition(int64_t regionID);
    expected<OfflineRegionStatus, std::exception_ptr> getRegionCompletedStatus(int64_t regionID);

    void setOfflineMapboxTileCountLimit(uint64_t);
    uint64_t getOfflineMapboxTileCountLimit();
    bool offlineMapboxTileCountLimitExceeded();
    uint64_t getOfflineMapboxTileCount();
    bool exceedsOfflineMapboxTileCountLimit(const Resource&);

private:
    void initialize();
    void handleError(const mapbox::sqlite::Exception&, const char* action);
    void handleError(const util::IOException&, const char* action);

    void removeExisting();
    void removeOldCacheTable();
    void createSchema();
    void migrateToVersion5();
    void migrateToVersion3();
    void migrateToVersion6();

    mapbox::sqlite::Statement& getStatement(const char *);

    optional<std::pair<Response, uint64_t>> getTile(const Resource::TileData&);
    optional<int64_t> hasTile(const Resource::TileData&);
    bool putTile(const Resource::TileData&, const Response&,
                 const std::string&, bool compressed);

    optional<std::pair<Response, uint64_t>> getResource(const Resource&);
    optional<int64_t> hasResource(const Resource&);
    bool putResource(const Resource&, const Response&,
                     const std::string&, bool compressed);

    uint64_t putRegionResourceInternal(int64_t regionID, const Resource&, const Response&);

    optional<std::pair<Response, uint64_t>> getInternal(const Resource&);
    optional<int64_t> hasInternal(const Resource&);
    std::pair<bool, uint64_t> putInternal(const Resource&, const Response&, bool evict);

    // Return value is true iff the resource was previously unused by any other regions.
    bool markUsed(int64_t regionID, const Resource&);

    std::pair<int64_t, int64_t> getCompletedResourceCountAndSize(int64_t regionID);
    std::pair<int64_t, int64_t> getCompletedTileCountAndSize(int64_t regionID);

    const std::string path;
    std::unique_ptr<mapbox::sqlite::Database> db;
    std::unordered_map<const char *, const std::unique_ptr<mapbox::sqlite::Statement>> statements;

    template <class T>
    T getPragma(const char *);

    uint64_t maximumCacheSize;

    uint64_t offlineMapboxTileCountLimit = util::mapbox::DEFAULT_OFFLINE_TILE_COUNT_LIMIT;
    optional<uint64_t> offlineMapboxTileCount;

    bool evict(uint64_t neededFreeSize);
};

} // namespace mbgl
