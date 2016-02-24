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
                    uint64_t maximumCacheSize = util::DEFAULT_MAX_CACHE_SIZE);
    ~OfflineDatabase();

    optional<Response> get(const Resource&);

    // Return value is (inserted, stored size)
    std::pair<bool, uint64_t> put(const Resource&, const Response&);

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

    optional<Response> getTile(const Resource::TileData&);
    bool putTile(const Resource::TileData&, const Response&,
                 const std::string&, bool compressed);

    optional<Response> getResource(const Resource&);
    bool putResource(const Resource&, const Response&,
                     const std::string&, bool compressed);

    std::pair<bool, uint64_t> putInternal(const Resource&, const Response&, bool evict);
    void markUsed(int64_t regionID, const Resource&);

    const std::string path;
    std::unique_ptr<::mapbox::sqlite::Database> db;
    std::unordered_map<const char *, std::unique_ptr<::mapbox::sqlite::Statement>> statements;

    template <class T>
    T getPragma(const char *);

    uint64_t maximumCacheSize;

    bool evict(uint64_t neededFreeSize);
};

} // namespace mbgl

#endif
