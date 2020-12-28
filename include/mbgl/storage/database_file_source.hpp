#pragma once

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/offline.hpp>
#include <mbgl/util/expected.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

class ResourceOptions;

// TODO: Split DatabaseFileSource into Ambient cache and Database interfaces.
class DatabaseFileSource : public FileSource {
public:
    explicit DatabaseFileSource(const ResourceOptions& options);
    ~DatabaseFileSource() override;

    // FileSource overrides
    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;
    void forward(const Resource&, const Response&, std::function<void()> callback) override;
    bool canRequest(const Resource&) const override;
    void setProperty(const std::string&, const mapbox::base::Value&) override;
    void pause() override;
    void resume() override;

    // Methods common to Ambient cache and Offline functionality

    /*
     * Sets path of a database to be used by DatabaseFileSource and invokes provided
     * callback when a database path is set.
     */
    virtual void setDatabasePath(const std::string&, std::function<void()> callback);

    /*
     * Delete existing database and re-initialize.
     *
     * When the operation is complete or encounters an error, the given callback will be
     * executed on the database thread; it is the responsibility of the SDK bindings
     * to re-execute a user-provided callback on the main thread.
     */
    virtual void resetDatabase(std::function<void(std::exception_ptr)>);

    /*
     * Packs the existing database file into a minimal amount of disk space.
     *
     * This operation has a performance impact as it will vacuum the database,
     * forcing it to move pages on the filesystem.
     *
     * When the operation is complete or encounters an error, the given callback will be
     * executed on the database thread; it is the responsibility of the SDK bindings
     * to re-execute a user-provided callback on the main thread.
     */
    virtual void packDatabase(std::function<void(std::exception_ptr)> callback);

    /*
     * Sets whether packing the database file occurs automatically after an offline
     * region is deleted (deleteOfflineRegion()) or the ambient cache is cleared
     * (clearAmbientCache()).
     *
     * By default, packing is enabled. If disabled, disk space will not be freed
     * after resources are removed unless packDatabase() is explicitly called.
     */
    virtual void runPackDatabaseAutomatically(bool);

    // Ambient cache

    /*
     * Insert the provided resource into the ambient cache
     *
     * Consumers of the resource will expect the uncompressed version; the
     * OfflineDatabase will determine whether to compress the data on disk.
     * This call is asynchronous: the data may not be immediately available
     * for in-progress requests, although subsequent requests should have
     * access to the cached data.
     */
    virtual void put(const Resource&, const Response&);

    /*
     * Forces revalidation of the ambient cache.
     *
     * Forces Mapbox GL Native to revalidate resources stored in the ambient
     * cache with the tile server before using them, making sure they
     * are the latest version. This is more efficient than cleaning the
     * cache because if the resource is considered valid after the server
     * lookup, it will not get downloaded again.
     *
     * Resources overlapping with offline regions will not be affected
     * by this call.
     */
    virtual void invalidateAmbientCache(std::function<void(std::exception_ptr)>);

    /*
     * Erase resources from the ambient cache, freeing storage space.
     *
     * Erases the ambient cache, freeing resources.
     *
     * Note that this operation can be potentially slow if packing the database
     * occurs automatically (see runPackDatabaseAutomatically() and packDatabase()).
     *
     * Resources overlapping with offline regions will not be affected
     * by this call.
     */
    virtual void clearAmbientCache(std::function<void(std::exception_ptr)>);

    /*
     * Sets the maximum size in bytes for the ambient cache.
     *
     * This call is potentially expensive because it will try
     * to trim the data in case the database is larger than the
     * size defined. The size of offline regions are not affected
     * by this settings, but the ambient cache will always try
     * to not exceed the maximum size defined, taking into account
     * the current size for the offline regions.
     *
     * If the maximum size is set to 50 MB and 40 MB are already
     * used by offline regions, the cache size will be effectively
     * 10 MB.
     *
     * Setting the size to 0 will disable the cache if there is no
     * offline region on the database.
     *
     * This method should always be called before using the database,
     * otherwise the default maximum size will be used.
     */
    virtual void setMaximumAmbientCacheSize(uint64_t size, std::function<void(std::exception_ptr)> callback);

    // Offline

    /*
     * Retrieve all regions in the offline database.
     *
     * The query will be executed asynchronously and the results passed to the given
     * callback, which will be executed on the database thread; it is the responsibility
     * of the SDK bindings to re-execute a user-provided callback on the main thread.
     */
    virtual void listOfflineRegions(std::function<void(expected<OfflineRegions, std::exception_ptr>)>);

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
    virtual void createOfflineRegion(const OfflineRegionDefinition& definition,
                                     const OfflineRegionMetadata& metadata,
                                     std::function<void(expected<OfflineRegion, std::exception_ptr>)>);
    /*
     * Update an offline region metadata in the database.
     */
    virtual void updateOfflineMetadata(int64_t regionID,
                                       const OfflineRegionMetadata& metadata,
                                       std::function<void(expected<OfflineRegionMetadata, std::exception_ptr>)>);

    /*
     * Register an observer to be notified when the state of the region changes.
     */
    virtual void setOfflineRegionObserver(const OfflineRegion&, std::unique_ptr<OfflineRegionObserver>);

    /*
     * Pause or resume downloading of regional resources.
     */
    virtual void setOfflineRegionDownloadState(const OfflineRegion&, OfflineRegionDownloadState);

    /*
     * Retrieve the current status of the region. The query will be executed
     * asynchronously and the results passed to the given callback, which will be
     * executed on the database thread; it is the responsibility of the SDK bindings
     * to re-execute a user-provided callback on the main thread.
     */
    virtual void getOfflineRegionStatus(const OfflineRegion&,
                                        std::function<void(expected<OfflineRegionStatus, std::exception_ptr>)>) const;

    /*
     * Merge offline regions from a secondary database into the main offline database.
     *
     * When the database merge is completed, the provided callback will be
     * executed on the database thread; it is the responsibility of the SDK bindings
     * to re-execute a user-provided callback on the main thread.
     *
     * The secondary database may need to be upgraded to the latest schema. This is done
     * in-place and requires write-access to `sideDatabasePath`; it is the
     * responsibility of the SDK bindings to ensure that this path is writeable.
     *
     * Only resources and tiles that belong to a region will be copied over. Identical
     * regions will be flattened into a single new region in the main database.
     *
     * Invokes the callback with a `MapboxOfflineTileCountExceededException` error if
     * the merge operation would result in the offline tile count limit being exceeded.
     *
     * Merged regions may not be in a completed status if the secondary database
     * does not contain all the tiles or resources required by the region definition.
     */
    virtual void mergeOfflineRegions(const std::string& sideDatabasePath,
                                     std::function<void(expected<OfflineRegions, std::exception_ptr>)>);

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
     * Note that this operation can be potentially slow if packing the database occurs
     * automatically (see runPackDatabaseAutomatically() and packDatabase()).
     *
     * When the operation is complete or encounters an error, the given callback will be
     * executed on the database thread; it is the responsibility of the SDK bindings
     * to re-execute a user-provided callback on the main thread.
     */
    virtual void deleteOfflineRegion(const OfflineRegion&, std::function<void(std::exception_ptr)>);

    /*
     * Invalidate all the tiles from an offline region forcing Mapbox GL to revalidate
     * the tiles with the server before using. This is more efficient than deleting the
     * offline region and downloading it again because if the data on the cache matches
     * the server, no new data gets transmitted.
     */
    virtual void invalidateOfflineRegion(const OfflineRegion&, std::function<void(std::exception_ptr)>);

    /*
     * Changing or bypassing this limit without permission from Mapbox is prohibited
     * by the Mapbox Terms of Service.
     */
    virtual void setOfflineMapboxTileCountLimit(uint64_t) const;

private:
    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace mbgl
