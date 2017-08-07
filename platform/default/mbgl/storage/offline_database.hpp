#pragma once

#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/offline.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mapbox.hpp>

#include <unordered_map>
#include <memory>
#include <string>

namespace mapbox {
namespace sqlite {
class Database;
class Statement;
} // namespace sqlite
} // namespace mapbox

namespace mbgl {

class Response;
class TileID;

class OfflineDatabase : private util::noncopyable {
public:
    // Limits affect ambient caching (put) only; resources required by offline
    // regions are exempt.
    OfflineDatabase(std::string path, uint64_t maximumCacheSize = util::DEFAULT_MAX_CACHE_SIZE);
    ~OfflineDatabase();

    optional<Response> get(const Resource&);

    // Return value is (inserted, stored size)
    std::pair<bool, uint64_t> put(const Resource&, const Response&);

    std::vector<OfflineRegion> listRegions();

    OfflineRegion createRegion(const OfflineRegionDefinition&,
                               const OfflineRegionMetadata&);

    OfflineRegionMetadata updateMetadata(const int64_t regionID, const OfflineRegionMetadata&);

    void deleteRegion(OfflineRegion&&);

    // Return value is (response, stored size)
    optional<std::pair<Response, uint64_t>> getRegionResource(int64_t regionID, const Resource&);
    optional<int64_t> hasRegionResource(int64_t regionID, const Resource&);
    uint64_t putRegionResource(int64_t regionID, const Resource&, const Response&);

    OfflineRegionDefinition getRegionDefinition(int64_t regionID);
    OfflineRegionStatus getRegionCompletedStatus(int64_t regionID);

    void setOfflineMapboxTileCountLimit(uint64_t);
    uint64_t getOfflineMapboxTileCountLimit();
    bool offlineMapboxTileCountLimitExceeded();
    uint64_t getOfflineMapboxTileCount();

private:
    void connect(int flags);
    int userVersion();
    void ensureSchema();
    void removeExisting();
    void migrateToVersion3();
    void migrateToVersion5();
    void migrateToVersion6();

    class Statement {
    public:
        explicit Statement(mapbox::sqlite::Statement& stmt_) : stmt(stmt_) {}
        Statement(Statement&&) = default;
        Statement(const Statement&) = delete;
        ~Statement();

        mapbox::sqlite::Statement* operator->() { return &stmt; };

    private:
        mapbox::sqlite::Statement& stmt;
    };

    Statement getStatement(const char *);

    optional<std::pair<Response, uint64_t>> getTile(const Resource::TileData&);
    optional<int64_t> hasTile(const Resource::TileData&);
    bool putTile(const Resource::TileData&, const Response&,
                 const std::string&, bool compressed);

    optional<std::pair<Response, uint64_t>> getResource(const Resource&);
    optional<int64_t> hasResource(const Resource&);
    bool putResource(const Resource&, const Response&,
                     const std::string&, bool compressed);

    optional<std::pair<Response, uint64_t>> getInternal(const Resource&);
    optional<int64_t> hasInternal(const Resource&);
    std::pair<bool, uint64_t> putInternal(const Resource&, const Response&, bool evict);

    // Return value is true iff the resource was previously unused by any other regions.
    bool markUsed(int64_t regionID, const Resource&);

    std::pair<int64_t, int64_t> getCompletedResourceCountAndSize(int64_t regionID);
    std::pair<int64_t, int64_t> getCompletedTileCountAndSize(int64_t regionID);

    const std::string path;
    std::unique_ptr<::mapbox::sqlite::Database> db;
    std::unordered_map<const char *, std::unique_ptr<::mapbox::sqlite::Statement>> statements;

    template <class T>
    T getPragma(const char *);

    uint64_t maximumCacheSize;

    uint64_t offlineMapboxTileCountLimit = util::mapbox::DEFAULT_OFFLINE_TILE_COUNT_LIMIT;
    optional<uint64_t> offlineMapboxTileCount;

    bool evict(uint64_t neededFreeSize);
};

} // namespace mbgl
