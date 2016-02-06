#ifndef MBGL_STORAGE_DEFAULT_FILE_SOURCE
#define MBGL_STORAGE_DEFAULT_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/offline.hpp>

#include <vector>

namespace mbgl {

namespace util {
template <typename T> class Thread;
} // namespace util

class DefaultFileSource : public FileSource {
public:
    DefaultFileSource(const std::string& cachePath, const std::string& assetRoot);
    ~DefaultFileSource() override;

    void setAccessToken(const std::string&);
    std::string getAccessToken() const;

    void setMaximumCacheSize(uint64_t size);
    void setMaximumCacheEntrySize(uint64_t size);

    std::unique_ptr<FileRequest> request(const Resource&, Callback) override;

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
     * Initiate the removal of offline region from the database.
     *
     * All resources required by the region, but not also required by other regions, will
     * become eligible for removal for space-optimization. Because the offline database is
     * also used for ambient usage-based caching, and offline resources may still be useful
     * for ambient usage, they are not immediately removed. To immediately remove resources
     * not used by any extant region, call removeUnusedOfflineResources().
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
     * Remove all resources in the database that are not required by any region, thus
     * optimizing the disk space used by the offline database.
     *
     * When the operation is complete or encounters an error, the given callback will be
     * executed on the database thread; it is the responsibility of the SDK bindings
     * to re-execute a user-provided callback on the main thread.
     */
    void removeUnusedOfflineResources(std::function<void (std::exception_ptr)>);

    // For testing only.
    void put(const Resource&, const Response&);
    void goOffline();

    class Impl;

private:
    const std::unique_ptr<util::Thread<Impl>> thread;
    const std::unique_ptr<FileSource> assetFileSource;
};

} // namespace mbgl

#endif
