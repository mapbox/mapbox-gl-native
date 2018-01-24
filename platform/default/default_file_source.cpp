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

#include <cassert>

namespace mbgl {

class DefaultFileSource::Impl {
public:
    Impl(ActorRef<Impl> self, std::shared_ptr<FileSource> assetFileSource_, const std::string& cachePath, uint64_t maximumCacheSize)
            : assetFileSource(assetFileSource_)
            , localFileSource(std::make_unique<LocalFileSource>()) {
        // Initialize the Database asynchronously so as to not block Actor creation.
        self.invoke(&Impl::initializeOfflineDatabase, cachePath, maximumCacheSize);
    }

    void initializeOfflineDatabase(std::string cachePath, uint64_t maximumCacheSize) {
        offlineDatabase = std::make_unique<OfflineDatabase>(cachePath, maximumCacheSize);
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

    void listRegions(std::function<void (std::exception_ptr, optional<std::vector<OfflineRegion>>)> callback) {
        try {
            callback({}, offlineDatabase->listRegions());
        } catch (...) {
            callback(std::current_exception(), {});
        }
    }

    void createRegion(const OfflineRegionDefinition& definition,
                      const OfflineRegionMetadata& metadata,
                      std::function<void (std::exception_ptr, optional<OfflineRegion>)> callback) {
        try {
            callback({}, offlineDatabase->createRegion(definition, metadata));
        } catch (...) {
            callback(std::current_exception(), {});
        }
    }

    void updateMetadata(const int64_t regionID,
                      const OfflineRegionMetadata& metadata,
                      std::function<void (std::exception_ptr, optional<OfflineRegionMetadata>)> callback) {
        try {
            callback({}, offlineDatabase->updateMetadata(regionID, metadata));
        } catch (...) {
            callback(std::current_exception(), {});
        }
    }

    void getRegionStatus(int64_t regionID, std::function<void (std::exception_ptr, optional<OfflineRegionStatus>)> callback) {
        try {
            callback({}, getDownload(regionID).getStatus());
        } catch (...) {
            callback(std::current_exception(), {});
        }
    }

    void deleteRegion(OfflineRegion&& region, std::function<void (std::exception_ptr)> callback) {
        try {
            downloads.erase(region.getID());
            offlineDatabase->deleteRegion(std::move(region));
            callback({});
        } catch (...) {
            callback(std::current_exception());
        }
    }

    void setRegionObserver(int64_t regionID, std::unique_ptr<OfflineRegionObserver> observer) {
        getDownload(regionID).setObserver(std::move(observer));
    }

    void setRegionDownloadState(int64_t regionID, OfflineRegionDownloadState state) {
        getDownload(regionID).setState(state);
    }

    void request(AsyncRequest* req, Resource resource, ActorRef<FileSourceRequest> ref) {
        auto callback = [ref] (const Response& res) mutable {
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
                tasks[req] = onlineFileSource.request(resource, [=] (Response onlineResponse) mutable {
                    this->offlineDatabase->put(resource, onlineResponse);
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

    void put(const Resource& resource, const Response& response) {
        offlineDatabase->put(resource, response);
    }

private:
    OfflineDownload& getDownload(int64_t regionID) {
        auto it = downloads.find(regionID);
        if (it != downloads.end()) {
            return *it->second;
        }
        return *downloads.emplace(regionID,
            std::make_unique<OfflineDownload>(regionID, offlineDatabase->getRegionDefinition(regionID), *offlineDatabase, onlineFileSource)).first->second;
    }

    // shared so that destruction is done on the creating thread
    const std::shared_ptr<FileSource> assetFileSource;
    const std::unique_ptr<FileSource> localFileSource;
    std::unique_ptr<OfflineDatabase> offlineDatabase;
    OnlineFileSource onlineFileSource;
    std::unordered_map<AsyncRequest*, std::unique_ptr<AsyncRequest>> tasks;
    std::unordered_map<int64_t, std::unique_ptr<OfflineDownload>> downloads;
};

DefaultFileSource::DefaultFileSource(const std::string& cachePath,
                                     const std::string& assetRoot,
                                     uint64_t maximumCacheSize)
    : DefaultFileSource(cachePath, std::make_unique<AssetFileSource>(assetRoot), maximumCacheSize) {
}

DefaultFileSource::DefaultFileSource(const std::string& cachePath,
                                     std::unique_ptr<FileSource>&& assetFileSource_,
                                     uint64_t maximumCacheSize)
        : assetFileSource(std::move(assetFileSource_))
        , impl(std::make_unique<util::Thread<Impl>>("DefaultFileSource", assetFileSource, cachePath, maximumCacheSize)) {
}

DefaultFileSource::~DefaultFileSource() = default;

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

std::unique_ptr<AsyncRequest> DefaultFileSource::request(const Resource& resource, Callback callback) {
    auto req = std::make_unique<FileSourceRequest>(std::move(callback));

    req->onCancel([fs = impl->actor(), req = req.get()] () mutable { fs.invoke(&Impl::cancel, req); });

    impl->actor().invoke(&Impl::request, req.get(), resource, req->actor());

    return std::move(req);
}

void DefaultFileSource::listOfflineRegions(std::function<void (std::exception_ptr, optional<std::vector<OfflineRegion>>)> callback) {
    impl->actor().invoke(&Impl::listRegions, callback);
}

void DefaultFileSource::createOfflineRegion(const OfflineRegionDefinition& definition,
                                            const OfflineRegionMetadata& metadata,
                                            std::function<void (std::exception_ptr, optional<OfflineRegion>)> callback) {
    impl->actor().invoke(&Impl::createRegion, definition, metadata, callback);
}

void DefaultFileSource::updateOfflineMetadata(const int64_t regionID,
                                            const OfflineRegionMetadata& metadata,
                                            std::function<void (std::exception_ptr, optional<OfflineRegionMetadata>)> callback) {
    impl->actor().invoke(&Impl::updateMetadata, regionID, metadata, callback);
}

void DefaultFileSource::deleteOfflineRegion(OfflineRegion&& region, std::function<void (std::exception_ptr)> callback) {
    impl->actor().invoke(&Impl::deleteRegion, std::move(region), callback);
}

void DefaultFileSource::setOfflineRegionObserver(OfflineRegion& region, std::unique_ptr<OfflineRegionObserver> observer) {
    impl->actor().invoke(&Impl::setRegionObserver, region.getID(), std::move(observer));
}

void DefaultFileSource::setOfflineRegionDownloadState(OfflineRegion& region, OfflineRegionDownloadState state) {
    impl->actor().invoke(&Impl::setRegionDownloadState, region.getID(), state);
}

void DefaultFileSource::getOfflineRegionStatus(OfflineRegion& region, std::function<void (std::exception_ptr, optional<OfflineRegionStatus>)> callback) const {
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

// For testing only:

void DefaultFileSource::setOnlineStatus(const bool status) {
    impl->actor().invoke(&Impl::setOnlineStatus, status);
}

void DefaultFileSource::put(const Resource& resource, const Response& response) {
    impl->actor().invoke(&Impl::put, resource, response);
}

} // namespace mbgl
