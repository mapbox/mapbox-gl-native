#pragma once

#include <mbgl/storage/offline.hpp>
#include <mbgl/storage/resource.hpp>

#include <list>
#include <unordered_set>
#include <memory>
#include <deque>

namespace mbgl {

class OfflineDatabase;
class FileSource;
class AsyncRequest;
class Response;
class Tileset;

namespace style {
class Parser;
} // namespace style

/**
 * Coordinates the request and storage of all resources for an offline region.

 * @private
 */
class OfflineDownload {
public:
    OfflineDownload(int64_t id, OfflineRegionDefinition&&, OfflineDatabase& offline, FileSource& online);
    ~OfflineDownload();

    void setObserver(std::unique_ptr<OfflineRegionObserver>);
    void setState(OfflineRegionDownloadState);

    OfflineRegionStatus getStatus() const;

private:
    void activateDownload();
    void continueDownload();
    void deactivateDownload();

    /*
     * Ensure that the resource is stored in the database, requesting it if necessary.
     * While the request is in progress, it is recorded in `requests`. If the download
     * is deactivated, all in progress requests are cancelled.
     */
    void ensureResource(const Resource&, std::function<void (Response)> = {});

    void onMapboxTileCountLimitExceeded();

    int64_t id;
    OfflineRegionDefinition definition;
    OfflineDatabase& offlineDatabase;
    FileSource& onlineFileSource;
    OfflineRegionStatus status;
    std::unique_ptr<OfflineRegionObserver> observer;

    std::list<std::unique_ptr<AsyncRequest>> requests;
    std::unordered_set<std::string> requiredSourceURLs;
    std::deque<Resource> resourcesRemaining;
    std::list<std::tuple<Resource, Response>> buffer;

    void queueResource(Resource);
    void queueTiles(style::SourceType, uint16_t tileSize, const Tileset&);
};

} // namespace mbgl
