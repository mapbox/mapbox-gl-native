#ifndef MBGL_OFFLINE_DATABASE
#define MBGL_OFFLINE_DATABASE

#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/offline.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/constants.hpp>

#include <unordered_map>
#include <memory>
#include <string>

namespace mapbox {
namespace sqlite {
class Database;
class Statement;
}
}

namespace mbgl {

class Response;
class TileID;

class OfflineDatabase : private util::noncopyable {
public:
    // Limits affect ambient caching (put) only; resources required by offline
    // regions are exempt.
    OfflineDatabase(const std::string& path,
                    uint64_t maximumCacheSize      = util::DEFAULT_MAX_CACHE_SIZE,
                    uint64_t maximumCacheEntrySize = util::DEFAULT_MAX_CACHE_ENTRY_SIZE);
    ~OfflineDatabase();

    optional<Response> get(const Resource&);
    uint64_t put(const Resource&, const Response&);

    std::vector<OfflineRegion> listRegions();

    OfflineRegion createRegion(const OfflineRegionDefinition&,
                               const OfflineRegionMetadata&);

    void deleteRegion(OfflineRegion&&);

    optional<Response> getRegionResource(int64_t regionID, const Resource&);
    uint64_t putRegionResource(int64_t regionID, const Resource&, const Response&);

    OfflineRegionDefinition getRegionDefinition(int64_t regionID);
    OfflineRegionStatus getRegionCompletedStatus(int64_t regionID);

private:
    void ensureSchema();
    void removeExisting();

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
    uint64_t putInternal(const Resource&, const Response&);

    optional<Response> getTile(const Resource::TileData&);
    uint64_t putTile(const Resource::TileData&, const Response&);

    optional<Response> getResource(const Resource&);
    uint64_t putResource(const Resource&, const Response&);

    void markUsed(int64_t regionID, const Resource&);

    const std::string path;
    std::unique_ptr<::mapbox::sqlite::Database> db;
    std::unordered_map<const char *, std::unique_ptr<::mapbox::sqlite::Statement>> statements;

    template <class T>
    T getPragma(const char *);

    uint64_t maximumCacheSize;
    uint64_t maximumCacheEntrySize;

    bool evict();
};

} // namespace mbgl

#endif
