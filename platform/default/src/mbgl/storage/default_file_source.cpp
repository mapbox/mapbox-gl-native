#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/file_source_request.hpp>
#include <mbgl/storage/local_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/offline_download.hpp>
#include <mbgl/storage/resource_transform.hpp>

#include <mbgl/util/platform.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/util/stopwatch.hpp>

#include <cassert>
#include <utility>

namespace mbgl {

class DefaultFileSource::Impl {
public:
    Impl(std::shared_ptr<FileSource> assetFileSource_, std::string cachePath)
            : assetFileSource(std::move(assetFileSource_))
            , localFileSource(std::make_unique<LocalFileSource>())
            , offlineDatabase(std::make_unique<OfflineDatabase>(std::move(cachePath))) {
    }

    void setAPIBaseURL(const std::string& url) {
        onlineFileSource.setAPIBaseURL(url);
    }

    std::string getAPIBaseURL() const{
        return onlineFileSource.getAPIBaseURL();
    }

    void setAccessToken(const std::string& accessToken) {
        onlineFileSource.setAccessToken(accessToken);
    }

    std::string getAccessToken() const {
        return onlineFileSource.getAccessToken();
    }

    void setResourceTransform(optional<ActorRef<ResourceTransform>>&& transform) {
        onlineFileSource.setResourceTransform(std::move(transform));
    }

    void setResourceCachePath(const std::string& path, optional<ActorRef<PathChangeCallback>>&& callback) {
        offlineDatabase->changePath(path);
        if (callback) {
            callback->invoke(&PathChangeCallback::operator());
        }
    }

    void listRegions(std::function<void (expected<OfflineRegions, std::exception_ptr>)> callback) {
        callback(offlineDatabase->listRegions());
    }

    void createRegion(const OfflineRegionDefinition& definition,
                      const OfflineRegionMetadata& metadata,
                      std::function<void (expected<OfflineRegion, std::exception_ptr>)> callback) {
        callback(offlineDatabase->createRegion(definition, metadata));
    }

    void mergeOfflineRegions(const std::string& sideDatabasePath,
                             std::function<void (expected<OfflineRegions, std::exception_ptr>)> callback) {
        callback(offlineDatabase->mergeDatabase(sideDatabasePath));
     }

    void updateMetadata(const int64_t regionID,
                      const OfflineRegionMetadata& metadata,
                      std::function<void (expected<OfflineRegionMetadata, std::exception_ptr>)> callback) {
        callback(offlineDatabase->updateMetadata(regionID, metadata));
    }

    void getRegionStatus(int64_t regionID, std::function<void (expected<OfflineRegionStatus, std::exception_ptr>)> callback) {
        if (auto download = getDownload(regionID)) {
            callback(download.value()->getStatus());
        } else {
            callback(unexpected<std::exception_ptr>(download.error()));
        }
    }

    void deleteRegion(OfflineRegion&& region, std::function<void(std::exception_ptr)> callback, bool pack) {
        downloads.erase(region.getID());
        callback(offlineDatabase->deleteRegion(std::move(region), pack));
    }

    void invalidateRegion(int64_t regionID, std::function<void (std::exception_ptr)> callback) {
        callback(offlineDatabase->invalidateRegion(regionID));
    }

    void setRegionObserver(int64_t regionID, std::unique_ptr<OfflineRegionObserver> observer) {
        if (auto download = getDownload(regionID)) {
            download.value()->setObserver(std::move(observer));
        }
    }

    void setRegionDownloadState(int64_t regionID, OfflineRegionDownloadState state) {
        if (auto download = getDownload(regionID)) {
            download.value()->setState(state);
        }
    }

    void request(AsyncRequest* req, Resource resource, ActorRef<FileSourceRequest> ref) {
        auto callback = [ref] (const Response& res) {
            ref.invoke(&FileSourceRequest::setResponse, res);
        };

        if (AssetFileSource::acceptsURL(resource.url)) {
            //Asset request
            tasks[req] = assetFileSource->request(resource, callback);
        } else if (LocalFileSource::acceptsURL(resource.url)) {
            //Local file request
            tasks[req] = localFileSource->request(resource, callback);
        } else {
            // Try the offline database
            if (resource.hasLoadingMethod(Resource::LoadingMethod::Cache)) {
                auto offlineResponse = offlineDatabase->get(resource);

                if (resource.loadingMethod == Resource::LoadingMethod::CacheOnly) {
                    if (!offlineResponse) {
                        // Ensure there's always a response that we can send, so the caller knows that
                        // there's no optional data available in the cache, when it's the only place
                        // we're supposed to load from.
                        offlineResponse.emplace();
                        offlineResponse->noContent = true;
                        offlineResponse->error = std::make_unique<Response::Error>(
                                Response::Error::Reason::NotFound, "Not found in offline database");
                    } else if (!offlineResponse->isUsable()) {
                        // Don't return resources the server requested not to show when they're stale.
                        // Even if we can't directly use the response, we may still use it to send a
                        // conditional HTTP request, which is why we're saving it above.
                        offlineResponse->error = std::make_unique<Response::Error>(
                            Response::Error::Reason::NotFound, "Cached resource is unusable");
                    }
                    callback(*offlineResponse);
                } else if (offlineResponse) {
                    // Copy over the fields so that we can use them when making a refresh request.
                    resource.priorModified = offlineResponse->modified;
                    resource.priorExpires = offlineResponse->expires;
                    resource.priorEtag = offlineResponse->etag;
                    resource.priorData = offlineResponse->data;

                    if (offlineResponse->isUsable()) {
                        callback(*offlineResponse);
                    }
                }
            }

            // Get from the online file source
            if (resource.hasLoadingMethod(Resource::LoadingMethod::Network)) {
                MBGL_TIMING_START(watch);
                tasks[req] = onlineFileSource.request(resource, [=] (Response onlineResponse) {
                    this->offlineDatabase->put(resource, onlineResponse);
                    if (resource.kind == Resource::Kind::Tile) {
                        // onlineResponse.data will be null if data not modified
                        MBGL_TIMING_FINISH(watch,
                                           " Action: " << "Requesting," <<
                                           " URL: " << resource.url.c_str() <<
                                           " Size: " << (onlineResponse.data != nullptr ? onlineResponse.data->size() : 0) << "B," <<
                                           " Time")
                    }
                    callback(onlineResponse);
                });
            }
        }
    }

    void cancel(AsyncRequest* req) {
        tasks.erase(req);
    }

    void setOfflineMapboxTileCountLimit(uint64_t limit) {
        offlineDatabase->setOfflineMapboxTileCountLimit(limit);
    }

    void setOnlineStatus(const bool status) {
        onlineFileSource.setOnlineStatus(status);
    }

    void setMaximumConcurrentRequests(uint32_t maximumConcurrentRequests_) {
        onlineFileSource.setMaximumConcurrentRequests(maximumConcurrentRequests_);
    }

    void put(const Resource& resource, const Response& response) {
        offlineDatabase->put(resource, response);
    }

    void resetDatabase(std::function<void (std::exception_ptr)> callback) {
        callback(offlineDatabase->resetDatabase());
    }

    void invalidateAmbientCache(std::function<void (std::exception_ptr)> callback) {
        callback(offlineDatabase->invalidateAmbientCache());
    }

    void clearAmbientCache(std::function<void (std::exception_ptr)> callback) {
        callback(offlineDatabase->clearAmbientCache());
    }

    void setMaximumAmbientCacheSize(uint64_t size, std::function<void (std::exception_ptr)> callback) {
        callback(offlineDatabase->setMaximumAmbientCacheSize(size));
    }

    void packDatabase(std::function<void(std::exception_ptr)> callback) { callback(offlineDatabase->pack()); }

private:
    expected<OfflineDownload*, std::exception_ptr> getDownload(int64_t regionID) {
        auto it = downloads.find(regionID);
        if (it != downloads.end()) {
            return it->second.get();
        }
        auto definition = offlineDatabase->getRegionDefinition(regionID);
        if (!definition) {
            return unexpected<std::exception_ptr>(definition.error());
        }
        auto download = std::make_unique<OfflineDownload>(regionID, std::move(definition.value()),
                                                          *offlineDatabase, onlineFileSource);
        return downloads.emplace(regionID, std::move(download)).first->second.get();
    }

    // shared so that destruction is done on the creating thread
    const std::shared_ptr<FileSource> assetFileSource;
    const std::unique_ptr<FileSource> localFileSource;
    std::unique_ptr<OfflineDatabase> offlineDatabase;
    OnlineFileSource onlineFileSource;
    std::unordered_map<AsyncRequest*, std::unique_ptr<AsyncRequest>> tasks;
    std::unordered_map<int64_t, std::unique_ptr<OfflineDownload>> downloads;
};

DefaultFileSource::DefaultFileSource(const std::string& cachePath, const std::string& assetPath, bool supportCacheOnlyRequests_)
    : DefaultFileSource(cachePath, std::make_unique<AssetFileSource>(assetPath), supportCacheOnlyRequests_) {
}

DefaultFileSource::DefaultFileSource(const std::string& cachePath, std::unique_ptr<FileSource>&& assetFileSource_, bool supportCacheOnlyRequests_)
        : assetFileSource(std::move(assetFileSource_))
        , impl(std::make_unique<util::Thread<Impl>>("DefaultFileSource", assetFileSource, cachePath))
        , supportCacheOnlyRequests(supportCacheOnlyRequests_) {
}

DefaultFileSource::~DefaultFileSource() = default;

bool DefaultFileSource::supportsCacheOnlyRequests() const {
    return supportCacheOnlyRequests;
}

void DefaultFileSource::setAPIBaseURL(const std::string& baseURL) {
    impl->actor().invoke(&Impl::setAPIBaseURL, baseURL);

    {
        std::lock_guard<std::mutex> lock(cachedBaseURLMutex);
        cachedBaseURL = baseURL;
    }
}

std::string DefaultFileSource::getAPIBaseURL() {
    std::lock_guard<std::mutex> lock(cachedBaseURLMutex);
    return cachedBaseURL;
}

void DefaultFileSource::setAccessToken(const std::string& accessToken) {
    impl->actor().invoke(&Impl::setAccessToken, accessToken);

    {
        std::lock_guard<std::mutex> lock(cachedAccessTokenMutex);
        cachedAccessToken = accessToken;
    }
}

std::string DefaultFileSource::getAccessToken() {
    std::lock_guard<std::mutex> lock(cachedAccessTokenMutex);
    return cachedAccessToken;
}

void DefaultFileSource::setResourceTransform(optional<ActorRef<ResourceTransform>>&& transform) {
    impl->actor().invoke(&Impl::setResourceTransform, std::move(transform));
}

void DefaultFileSource::setResourceCachePath(const std::string& path, optional<ActorRef<PathChangeCallback>>&& callback) {
    impl->actor().invoke(&Impl::setResourceCachePath, path, std::move(callback));
}

std::unique_ptr<AsyncRequest> DefaultFileSource::request(const Resource& resource, Callback callback) {
    auto req = std::make_unique<FileSourceRequest>(std::move(callback));

    req->onCancel([fs = impl->actor(), req = req.get()] () { fs.invoke(&Impl::cancel, req); });

    impl->actor().invoke(&Impl::request, req.get(), resource, req->actor());

    return std::move(req);
}

void DefaultFileSource::listOfflineRegions(std::function<void (expected<OfflineRegions, std::exception_ptr>)> callback) {
    impl->actor().invoke(&Impl::listRegions, callback);
}

void DefaultFileSource::createOfflineRegion(const OfflineRegionDefinition& definition,
                                            const OfflineRegionMetadata& metadata,
                                            std::function<void (expected<OfflineRegion, std::exception_ptr>)> callback) {
    impl->actor().invoke(&Impl::createRegion, definition, metadata, callback);
}

void DefaultFileSource::mergeOfflineRegions(const std::string& sideDatabasePath,
                                            std::function<void (expected<OfflineRegions, std::exception_ptr>)> callback) {
    impl->actor().invoke(&Impl::mergeOfflineRegions, sideDatabasePath, callback);
}

void DefaultFileSource::updateOfflineMetadata(const int64_t regionID,
                                            const OfflineRegionMetadata& metadata,
                                            std::function<void (expected<OfflineRegionMetadata,
                                             std::exception_ptr>)> callback) {
    impl->actor().invoke(&Impl::updateMetadata, regionID, metadata, callback);
}

void DefaultFileSource::deleteOfflineRegion(OfflineRegion&& region,
                                            std::function<void(std::exception_ptr)> callback,
                                            bool pack) {
    impl->actor().invoke(&Impl::deleteRegion, std::move(region), callback, pack);
}

void DefaultFileSource::invalidateOfflineRegion(OfflineRegion& region,
                                                std::function<void(std::exception_ptr)> callback) {
    impl->actor().invoke(&Impl::invalidateRegion, region.getID(), callback);
}

void DefaultFileSource::setOfflineRegionObserver(OfflineRegion& region, std::unique_ptr<OfflineRegionObserver> observer) {
    impl->actor().invoke(&Impl::setRegionObserver, region.getID(), std::move(observer));
}

void DefaultFileSource::setOfflineRegionDownloadState(OfflineRegion& region, OfflineRegionDownloadState state) {
    impl->actor().invoke(&Impl::setRegionDownloadState, region.getID(), state);
}

void DefaultFileSource::getOfflineRegionStatus(OfflineRegion& region, std::function<void (expected<OfflineRegionStatus, std::exception_ptr>)> callback) const {
    impl->actor().invoke(&Impl::getRegionStatus, region.getID(), callback);
}

void DefaultFileSource::setOfflineMapboxTileCountLimit(uint64_t limit) const {
    impl->actor().invoke(&Impl::setOfflineMapboxTileCountLimit, limit);
}

void DefaultFileSource::pause() {
    impl->pause();
}

void DefaultFileSource::resume() {
    impl->resume();
}
    
void DefaultFileSource::put(const Resource& resource, const Response& response) {
    impl->actor().invoke(&Impl::put, resource, response);
}

void DefaultFileSource::resetDatabase(std::function<void (std::exception_ptr)> callback) {
    impl->actor().invoke(&Impl::resetDatabase, std::move(callback));
}

void DefaultFileSource::packDatabase(std::function<void(std::exception_ptr)> callback) {
    impl->actor().invoke(&Impl::packDatabase, std::move(callback));
}

void DefaultFileSource::invalidateAmbientCache(std::function<void (std::exception_ptr)> callback) {
    impl->actor().invoke(&Impl::invalidateAmbientCache, std::move(callback));
}

void DefaultFileSource::clearAmbientCache(std::function<void(std::exception_ptr)> callback) {
    impl->actor().invoke(&Impl::clearAmbientCache, std::move(callback));
}

void DefaultFileSource::setMaximumAmbientCacheSize(uint64_t size, std::function<void (std::exception_ptr)> callback) {
    impl->actor().invoke(&Impl::setMaximumAmbientCacheSize, size, std::move(callback));
}

// For testing only:

void DefaultFileSource::setOnlineStatus(const bool status) {
    impl->actor().invoke(&Impl::setOnlineStatus, status);
}

void DefaultFileSource::setMaximumConcurrentRequests(uint32_t maximumConcurrentRequests_) {
    impl->actor().invoke(&Impl::setMaximumConcurrentRequests, maximumConcurrentRequests_);
}

} // namespace mbgl
