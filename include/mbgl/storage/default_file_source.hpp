#ifndef MBGL_STORAGE_DEFAULT_FILE_SOURCE
#define MBGL_STORAGE_DEFAULT_FILE_SOURCE

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/geo.hpp>

#include <vector>

namespace mbgl {

/*
 * An offline region defined by a style URL, geographic bounding box, zoom range, and
 * device pixel ratio.
 */
class OfflineTilePyramidRegionDefinition {
public:
    OfflineTilePyramidRegionDefinition(const std::string&, const LatLngBounds&, double, double, float);

    std::string styleURL;
    LatLngBounds bounds;
    double minZoom;
    double maxZoom;
    float pixelRatio;
};

/*
 * For the present, a tile pyramid is the only type of offline region. In the future,
 * other definition types will be available and this will be a variant type.
 */
using OfflineRegionDefinition = OfflineTilePyramidRegionDefinition;

/*
 * Arbitrary binary region metadata. The contents are opaque to the mbgl implementation;
 * it just stores and retrieves a BLOB. SDK bindings may enforce a higher-level data format
 * if doing so is natural for the platform.
 */
using OfflineRegionMetadata = std::vector<uint8_t>;

/*
 * A region is either inactive (not downloading, but previously-downloaded
 * resources are available for use), or active (resources are being downloaded
 * or will be downloaded, if necessary, when network access is available).
 * 
 * This state is independent of whether or not the complete set of resources
 * is currently available for offline use. To check if that is the case, use
 * `OfflineRegionStatus::complete()`.
 */
enum class OfflineRegionDownloadState {
    Inactive,
    Active
};

/*
 * A region's status includes its active/inactive state as well as counts
 * of the number of resources that have completed downloading, their total
 * size in bytes, and the total number of resources that are required.
 * 
 * Note that the total required size in bytes is not currently available. A
 * future API release may provide an estimate of this number.
 */
class OfflineRegionStatus {
public:
    OfflineRegionDownloadState downloadState;
    uint64_t completedResourceCount;
    uint64_t completedResourceSize;
    uint64_t requiredResourceCount;

    bool complete() const {
        return completedResourceCount == requiredResourceCount;
    }
};

/*
 * A region can have a single observer, which gets notified whenever a change
 * to the region's status occurs.
 */
class OfflineRegionObserver {
public:
    virtual ~OfflineRegionObserver();

    /*
     * Implement this method to be notified of a change in the status of an
     * offline region. Status changes include any change in state of the members
     * of OfflineRegionStatus.
     *
     * Note that this method will be executed on the database thread; it is the 
     * responsibility of the SDK bindings to wrap this object in an interface that
     * re-executes the user-provided implementation on the main thread.
     */
    virtual void statusChanged(OfflineRegionStatus) {};

    /*
     * Implement this method to be notified of errors encountered while downloading
     * regional resources. Such errors may be recoverable; for example the implementation
     * will attempt to re-request failed resources based on an exponential backoff
     * algorithm, or when it detects that network access has been restored.
     *
     * Note that this method will be executed on the database thread; it is the 
     * responsibility of the SDK bindings to wrap this object in an interface that
     * re-executes the user-provided implementation on the main thread.
     */
    virtual void error(std::exception_ptr) {};
};

class OfflineRegion {
public:
    // Move-only; not publicly constructible.
    OfflineRegion(OfflineRegion&&) = default;
    OfflineRegion& operator=(OfflineRegion&&) = default;
    OfflineRegion() = delete;
    OfflineRegion(const OfflineRegion&) = delete;
    OfflineRegion& operator=(const OfflineRegion&) = delete;

    void setObserver(std::unique_ptr<OfflineRegionObserver>);
    void setDownloadState(OfflineRegionDownloadState);

    const OfflineRegionDefinition& getDefinition() const;
    const OfflineRegionMetadata& getMetadata() const;

    /*
     * Retrieve the current status of this region. The query will be executed
     * asynchronously and the results passed to the given callback, which will be
     * executed on the database thread; it is the responsibility of the SDK bindings
     * to re-execute a user-provided callback on the main thread.
     */
    void getStatus(std::function<void (std::exception_ptr,
                                       optional<OfflineRegionStatus>)>) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

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
     * downloading resources, call `setDownloadState(OfflineRegionDownloadState::Active)`,
     * optionally registering an `OfflineRegionObserver` beforehand.
     */
    void createOfflineRegion(const OfflineRegionDefinition& definition,
                             const OfflineRegionMetadata& metadata,
                             std::function<void (std::exception_ptr,
                                                 optional<OfflineRegion>)>);

    /*
     * Initiate the removal of offline region from the database. All resources required by
     * the region, but not also required by other regions, will be deleted.
     *
     * If an observer is registered for the region, it will receive status notifications
     * as the deletion progresses.
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

private:
    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace mbgl

#endif
