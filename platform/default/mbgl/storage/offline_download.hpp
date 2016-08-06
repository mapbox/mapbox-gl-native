#pragma once

#include <mbgl/storage/offline.hpp>

#include <list>
#include <set>
#include <memory>

namespace mbgl {

class OfflineDatabase;
class FileSource;
class AsyncRequest;
class Resource;
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
    void deactivateDownload();

    std::vector<Resource> spriteResources(const style::Parser&) const;
    std::vector<Resource> glyphResources(const style::Parser&) const;
    std::vector<Resource> tileResources(SourceType, uint16_t, const Tileset&) const;

    /*
     * Ensure that the resource is stored in the database, requesting it if necessary.
     * While the request is in progress, it is recorded in `requests`. If the download
     * is deactivated, all in progress requests are cancelled.
     */
    void ensureResource(const Resource&, std::function<void (Response)> = {});
    void ensureTiles(SourceType, uint16_t, const Tileset&);
    bool checkTileCountLimit(const Resource& resource);
    
    int64_t id;
    OfflineRegionDefinition definition;
    OfflineDatabase& offlineDatabase;
    FileSource& onlineFileSource;
    OfflineRegionStatus status;
    std::unique_ptr<OfflineRegionObserver> observer;
    std::list<std::unique_ptr<AsyncRequest>> requests;
    std::set<std::string> requiredSourceURLs;
};

} // namespace mbgl
