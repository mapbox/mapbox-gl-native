#pragma once

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/offline.hpp>
#include <mbgl/util/constants.hpp>

#include <vector>

namespace mbgl {

namespace util {
template <typename T> class Thread;
} // namespace util

class DefaultFileSource : public FileSource {
public:
    /*
     * The maximumCacheSize parameter is a limit applied to non-offline resources only,
     * i.e. resources added to the database for the "ambient use" caching functionality.
     * There is no size limit for offline resources. If a user never creates any offline
     * regions, we want the database to remain fairly small (order tens or low hundreds
     * of megabytes).
     */
    DefaultFileSource(const std::string& cachePath,
                      const std::string& assetRoot,
                      uint64_t maximumCacheSize = util::DEFAULT_MAX_CACHE_SIZE);
    ~DefaultFileSource() override;

    bool supportsOptionalRequests() const override {
        return true;
    }

    void setAPIBaseURL(const std::string&);
    std::string getAPIBaseURL() const;

    void setAccessToken(const std::string&);
    std::string getAccessToken() const;

    void setResourceTransform(std::function<std::string(Resource::Kind, std::string&&)>);

    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;

    /*
     * Retrieve all regions in the offline database.
     *
     * The query will be executed asynchronously and the results passed to the given
     * callback, which will be executed on the database thread; it is the responsibility
     * of the SDK bindings to re-execute a user-provided callback on the main thread.
     */
    void listOfflineRegions(std::function<void (std::exception_ptr,
                                                optional<std::vector<OfflineRegion>>)>);

    /*
     * Create an offline region in the database.
     *
     * When the initial database queries have completed, the provided callback will be
     * executed on the database thread; it is the responsibility of the SDK bindings
     * to re-execute a user-provided callback on the main thread.
     *
     * Note that the resulting region will be in an inactive download state; to begin
     * downloading resources, call `setOfflineRegionDownloadState(OfflineRegionDownloadState::Active)`,
     * optionally registering an `OfflineRegionObserver` beforehand.
     */
    void createOfflineRegion(const OfflineRegionDefinition& definition,
                             const OfflineRegionMetadata& metadata,
                             std::function<void (std::exception_ptr,
                                                 optional<OfflineRegion>)>);

    /*
     * Update an offline region metadata in the database.
     */
    void updateOfflineMetadata(const int64_t regionID,
                               const OfflineRegionMetadata& metadata,
                               std::function<void (std::exception_ptr,
                                                   optional<OfflineRegionMetadata>)>);
    /*
     * Register an observer to be notified when the state of the region changes.
     */
    void setOfflineRegionObserver(OfflineRegion&, std::unique_ptr<OfflineRegionObserver>);

    /*
     * Pause or resume downloading of regional resources.
     */
    void setOfflineRegionDownloadState(OfflineRegion&, OfflineRegionDownloadState);

    /*
     * Retrieve the current status of the region. The query will be executed
     * asynchronously and the results passed to the given callback, which will be
     * executed on the database thread; it is the responsibility of the SDK bindings
     * to re-execute a user-provided callback on the main thread.
     */
    void getOfflineRegionStatus(OfflineRegion&, std::function<void (std::exception_ptr,
                                                                    optional<OfflineRegionStatus>)>) const;

    /*
     * Remove an offline region from the database and perform any resources evictions
     * necessary as a result.
     *
     * Eviction works by removing the least-recently requested resources not also required
     * by other regions, until the database shrinks below a certain size.
     *
     * Note that this method takes ownership of the input, reflecting the fact that once
     * region deletion is initiated, it is not legal to perform further actions with the
     * region.
     *
     * When the operation is complete or encounters an error, the given callback will be
     * executed on the database thread; it is the responsibility of the SDK bindings
     * to re-execute a user-provided callback on the main thread.
     */
    void deleteOfflineRegion(OfflineRegion&&, std::function<void (std::exception_ptr)>);

    /*
     * Changing or bypassing this limit without permission from Mapbox is prohibited
     * by the Mapbox Terms of Service.
     */
    void setOfflineMapboxTileCountLimit(uint64_t) const;

    void pause();
    void resume();

    // For testing only.
    void put(const Resource&, const Response&);

    class Impl;

private:
    const std::unique_ptr<util::Thread<Impl>> thread;
    const std::unique_ptr<FileSource> assetFileSource;
    const std::unique_ptr<FileSource> localFileSource;
};

} // namespace mbgl
