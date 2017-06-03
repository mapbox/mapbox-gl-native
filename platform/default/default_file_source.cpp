#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/local_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/offline_download.hpp>

#include <mbgl/actor/actor.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/threaded_run_loop.hpp>

#include <cassert>

namespace {

const std::string assetProtocol = "asset://";

bool isAssetURL(const std::string& url) {
    return std::equal(assetProtocol.begin(), assetProtocol.end(), url.begin());
}

} // namespace

namespace mbgl {

class RequestCallback {
public:
    RequestCallback(ActorRef<RequestCallback>, FileSource::Callback callback_)
            : callback(std::move(callback_)) {
    }

    void operator() (Response response) {
        Log::Info(Event::General, "RequestCallback::operator()");
        callback(response);
    }

//    void operator() (Response response) const {
//        Log::Info(Event::General, "RequestCallback::operator() const");
//        callback(response);
//    }

private:
    FileSource::Callback callback;
};

//class ResourceTransformCallback {
//public:
//    ResourceTransformCallback(ActorRef<ResourceTransformCallback>, OnlineFileSource::ResourceTransform callback_)
//            : callback(std::move(callback_)) {
//    }
//
//    //std::function<std::unique_ptr<AsyncRequest>(Resource::Kind, std::string&&, std::function<void(std::string&&)>)>
//    std::unique_ptr<AsyncRequest> operator() (Resource::Kind kind, std::string&& url, std::function<void(std::string&&)> resultCallback) {
//        return callback(kind, std::move(url), resultCallback);
//    }
//
//private:
//    OnlineFileSource::ResourceTransform callback;
//};

//class AssetRequest {
//public:
//    AssetRequest(ActorRef<AssetRequest>);
//    ~AssetRequest(); // cancels
//
//    void send(const Resource& resource, ActorRef<RequestCallback> callback) {
//        // do whatever, then eventually:
//        callback.invoke(&RequestCallback::operator(), response);
//    }
//};

class DefaultFileSource::Impl {
public:
    Impl(ActorRef<Impl> /*self*/, std::unique_ptr<FileSource> assetFileSource_, const std::string& cachePath, uint64_t maximumCacheSize)
            : assetFileSource(std::move(assetFileSource_))
            , localFileSource(std::make_unique<LocalFileSource>())
            , offlineDatabase(cachePath, maximumCacheSize) {
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

    void setResourceTransform(OnlineFileSource::ResourceTransform&& transform) {
        onlineFileSource.setResourceTransform(std::move(transform));
    }

    void listRegions(std::function<void (std::exception_ptr, optional<std::vector<OfflineRegion>>)> callback) {
        try {
            callback({}, offlineDatabase.listRegions());
        } catch (...) {
            callback(std::current_exception(), {});
        }
    }

    void createRegion(const OfflineRegionDefinition& definition,
                      const OfflineRegionMetadata& metadata,
                      std::function<void (std::exception_ptr, optional<OfflineRegion>)> callback) {
        try {
            callback({}, offlineDatabase.createRegion(definition, metadata));
        } catch (...) {
            callback(std::current_exception(), {});
        }
    }

    void updateMetadata(const int64_t regionID,
                      const OfflineRegionMetadata& metadata,
                      std::function<void (std::exception_ptr, optional<OfflineRegionMetadata>)> callback) {
        try {
            callback({}, offlineDatabase.updateMetadata(regionID, metadata));
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
            offlineDatabase.deleteRegion(std::move(region));
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

    void request(AsyncRequest* req, Resource resource, ActorRef<RequestCallback> callback) {
        if (isAssetURL(resource.url)) {
            //Asset request
            tasks[req] = assetFileSource->request(resource, [&](Response response) { callback.invoke(&RequestCallback::operator(), std::move(response)); });
        } else if (LocalFileSource::acceptsURL(resource.url)) {
            //Local file request
            tasks[req] = localFileSource->request(resource, [&callback](Response response) { callback.invoke(&RequestCallback::operator(), response); });
        } else {
            // Try the offline database
            Resource revalidation = resource;

            const bool hasPrior = resource.priorEtag || resource.priorModified || resource.priorExpires;
            if (!hasPrior || resource.necessity == Resource::Optional) {
                auto offlineResponse = offlineDatabase.get(resource);

                if (resource.necessity == Resource::Optional && !offlineResponse) {
                    // Ensure there's always a response that we can send, so the caller knows that
                    // there's no optional data available in the cache.
                    offlineResponse.emplace();
                    offlineResponse->noContent = true;
                    offlineResponse->error = std::make_unique<Response::Error>(
                            Response::Error::Reason::NotFound, "Not found in offline database");
                }

                if (offlineResponse) {
                    revalidation.priorModified = offlineResponse->modified;
                    revalidation.priorExpires = offlineResponse->expires;
                    revalidation.priorEtag = offlineResponse->etag;
                    callback.invoke(&RequestCallback::operator(), *offlineResponse);
                }
            }

            // Get from the online file source
            if (resource.necessity == Resource::Required) {
                tasks[req] = onlineFileSource.request(revalidation, [=] (Response onlineResponse) {
                    this->offlineDatabase.put(revalidation, onlineResponse);
                    //TODO callback.invoke(&RequestCallback::operator(), onlineResponse);
                });
            }
        }
    }

    void cancel(AsyncRequest* req) {
        Log::Info(Event::General, "DefaultFileSource::Impl::cancel");
        tasks.erase(req);
    }

    void setOfflineMapboxTileCountLimit(uint64_t limit) {
        offlineDatabase.setOfflineMapboxTileCountLimit(limit);
    }

    void put(const Resource& resource, const Response& response) {
        offlineDatabase.put(resource, response);
    }

private:
    OfflineDownload& getDownload(int64_t regionID) {
        auto it = downloads.find(regionID);
        if (it != downloads.end()) {
            return *it->second;
        }
        return *downloads.emplace(regionID,
            std::make_unique<OfflineDownload>(regionID, offlineDatabase.getRegionDefinition(regionID), offlineDatabase, onlineFileSource)).first->second;
    }

    const std::unique_ptr<FileSource> assetFileSource;
    const std::unique_ptr<FileSource> localFileSource;
    OfflineDatabase offlineDatabase;
    OnlineFileSource onlineFileSource;
    std::unordered_map<AsyncRequest*, std::unique_ptr<AsyncRequest>> tasks;
    std::unordered_map<int64_t, std::unique_ptr<OfflineDownload>> downloads;
};

DefaultFileSource::DefaultFileSource(Scheduler& scheduler,
                                     const std::string& cachePath,
                                     const std::string& assetRoot,
                                     uint64_t maximumCacheSize)
    : DefaultFileSource(scheduler, cachePath, std::make_unique<AssetFileSource>(scheduler, assetRoot), maximumCacheSize) {
}

DefaultFileSource::DefaultFileSource(Scheduler& scheduler_,
                                     const std::string& cachePath,
                                     std::unique_ptr<FileSource>&& assetFileSource_,
                                     uint64_t maximumCacheSize)
        : scheduler(scheduler_)
        , workerScheduler(std::make_unique<ThreadedRunLoop>("DefaultFileSource"))
        , worker(std::make_unique<Actor<Impl>>(*workerScheduler, std::move(assetFileSource_), cachePath, maximumCacheSize)) {

}

DefaultFileSource::~DefaultFileSource() = default;

void DefaultFileSource::setAPIBaseURL(const std::string& baseURL) {
    worker->invoke(&Impl::setAPIBaseURL, baseURL);

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
    worker->invoke(&Impl::setAccessToken, accessToken);

    {
        std::lock_guard<std::mutex> lock(cachedAccessTokenMutex);
        cachedAccessToken = accessToken;
    }
}

std::string DefaultFileSource::getAccessToken() {
    std::lock_guard<std::mutex> lock(cachedAccessTokenMutex);
    return cachedAccessToken;
}

void DefaultFileSource::setResourceTransform(std::function<std::string(Resource::Kind, std::string&&)> transform) {
    if (transform) {
        auto loop = util::RunLoop::Get();
        worker->invoke(&Impl::setResourceTransform, [loop, transform](Resource::Kind kind_, std::string&& url_, auto callback_) {
            return loop->invokeWithCallback([transform](Resource::Kind kind, std::string&& url, auto callback) {
                callback(transform(kind, std::move(url)));
            }, kind_, std::move(url_), callback_);
        });
    } else {
        worker->invoke(&Impl::setResourceTransform, nullptr);
    }
}

std::unique_ptr<AsyncRequest> DefaultFileSource::request(const Resource& resource, Callback callback) {
    Log::Info(Event::General, "DefaultFileSource::request: %s", resource.url.c_str());
    class DefaultFileRequest : public AsyncRequest {
    public:
        DefaultFileRequest(Scheduler& scheduler, Resource resource_, FileSource::Callback callback_, ActorRef<DefaultFileSource::Impl> worker_)
            : worker(worker_)
            , callback(scheduler, callback_) {
            worker.invoke(&DefaultFileSource::Impl::request, this, resource_, callback.self());
        }

        ~DefaultFileRequest() override {
            worker.invoke(&DefaultFileSource::Impl::cancel, this);
        }

    private:
        ActorRef<DefaultFileSource::Impl> worker;
        Actor<RequestCallback> callback;
    };

    return std::make_unique<DefaultFileRequest>(scheduler, resource, callback, worker->self());
}

void DefaultFileSource::listOfflineRegions(std::function<void (std::exception_ptr, optional<std::vector<OfflineRegion>>)> callback) {
    worker->invoke(&Impl::listRegions, callback);
}

void DefaultFileSource::createOfflineRegion(const OfflineRegionDefinition& definition,
                                            const OfflineRegionMetadata& metadata,
                                            std::function<void (std::exception_ptr, optional<OfflineRegion>)> callback) {
    worker->invoke(&Impl::createRegion, definition, metadata, callback);
}

void DefaultFileSource::updateOfflineMetadata(const int64_t regionID,
                                            const OfflineRegionMetadata& metadata,
                                            std::function<void (std::exception_ptr, optional<OfflineRegionMetadata>)> callback) {
    worker->invoke(&Impl::updateMetadata, regionID, metadata, callback);
}

void DefaultFileSource::deleteOfflineRegion(OfflineRegion&& region, std::function<void (std::exception_ptr)> callback) {
    worker->invoke(&Impl::deleteRegion, std::move(region), callback);
}

void DefaultFileSource::setOfflineRegionObserver(OfflineRegion& region, std::unique_ptr<OfflineRegionObserver> observer) {
    worker->invoke(&Impl::setRegionObserver, region.getID(), std::move(observer));
}

void DefaultFileSource::setOfflineRegionDownloadState(OfflineRegion& region, OfflineRegionDownloadState state) {
    worker->invoke(&Impl::setRegionDownloadState, region.getID(), state);
}

void DefaultFileSource::getOfflineRegionStatus(OfflineRegion& region, std::function<void (std::exception_ptr, optional<OfflineRegionStatus>)> callback) const {
    worker->invoke(&Impl::getRegionStatus, region.getID(), callback);
}

void DefaultFileSource::setOfflineMapboxTileCountLimit(uint64_t limit) const {
    // TODO: was sync, why?
    worker->invoke(&Impl::setOfflineMapboxTileCountLimit, limit);
}

void DefaultFileSource::pause() {
    //TODO: thread->pause();
}

void DefaultFileSource::resume() {
    //TODO: thread->resume();
}

// For testing only:

void DefaultFileSource::put(const Resource& resource, const Response& response) {
    // TODO: was sync, callback?
    worker->invoke(&Impl::put, resource, response);
}

} // namespace mbgl
