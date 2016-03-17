    #pragma once

#include <mbgl/storage/offline.hpp>
#include <mbgl/style/types.hpp>

#include <list>
#include <set>
#include <memory>

namespace mbgl {

class OfflineDatabase;
class FileSource;
class WorkRequest;
class FileRequest;
class Resource;
class Response;
class SourceInfo;
class StyleParser;
class Source;

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

    std::vector<Resource> spriteResources(const StyleParser&) const;
    std::vector<Resource> glyphResources(const StyleParser&) const;
    std::vector<Resource> tileResources(SourceType, uint16_t, const SourceInfo&) const;

    /*
     * Ensure that the resource is stored in the database, requesting it if necessary.
     * While the request is in progress, it is recorded in `requests`. If the download
     * is deactivated, all in progress requests are cancelled.
     */
    void ensureResource(const Resource&, std::function<void (Response)> = {});
    void ensureTiles(SourceType, uint16_t, const SourceInfo&);
    bool checkTileCountLimit(const Resource& resource);
    
    int64_t id;
    OfflineRegionDefinition definition;
    OfflineDatabase& offlineDatabase;
    FileSource& onlineFileSource;
    OfflineRegionStatus status;
    std::unique_ptr<OfflineRegionObserver> observer;
    std::list<std::unique_ptr<WorkRequest>> workRequests;
    std::list<std::unique_ptr<FileRequest>> fileRequests;
    std::set<std::string> requiredSourceURLs;
};

} // namespace mbgl
