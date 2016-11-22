#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/local_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/offline_download.hpp>

#include <mbgl/util/platform.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/work_request.hpp>

#include <cassert>

namespace {

const std::string assetProtocol = "asset://";

bool isAssetURL(const std::string& url) {
    return std::equal(assetProtocol.begin(), assetProtocol.end(), url.begin());
}

} // namespace

namespace mbgl {

class DefaultFileSource::Impl {
public:
    Impl(const std::string& cachePath, uint64_t maximumCacheSize)
        : offlineDatabase(cachePath, maximumCacheSize) {
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

    void request(AsyncRequest* req, Resource resource, Callback callback) {
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
                callback(*offlineResponse);
            }
        }

        if (resource.necessity == Resource::Required) {
            tasks[req] = onlineFileSource.request(revalidation, [=] (Response onlineResponse) {
                this->offlineDatabase.put(revalidation, onlineResponse);
                callback(onlineResponse);
            });
        }
    }

    void cancel(AsyncRequest* req) {
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

    OfflineDatabase offlineDatabase;
    OnlineFileSource onlineFileSource;
    std::unordered_map<AsyncRequest*, std::unique_ptr<AsyncRequest>> tasks;
    std::unordered_map<int64_t, std::unique_ptr<OfflineDownload>> downloads;
};

DefaultFileSource::DefaultFileSource(const std::string& cachePath,
                                     const std::string& assetRoot,
                                     uint64_t maximumCacheSize)
    : thread(std::make_unique<util::Thread<Impl>>(util::ThreadContext{"DefaultFileSource", util::ThreadPriority::Low},
            cachePath, maximumCacheSize)),
      assetFileSource(std::make_unique<AssetFileSource>(assetRoot)),
      localFileSource(std::make_unique<LocalFileSource>()) {
}

DefaultFileSource::~DefaultFileSource() = default;

void DefaultFileSource::setAPIBaseURL(const std::string& baseURL) {
    thread->invokeSync(&Impl::setAPIBaseURL, baseURL);
}
    
std::string DefaultFileSource::getAPIBaseURL() const {
    return thread->invokeSync(&Impl::getAPIBaseURL);
}
    
void DefaultFileSource::setAccessToken(const std::string& accessToken) {
    thread->invokeSync(&Impl::setAccessToken, accessToken);
}

std::string DefaultFileSource::getAccessToken() const {
    return thread->invokeSync(&Impl::getAccessToken);
}

std::unique_ptr<AsyncRequest> DefaultFileSource::request(const Resource& resource, Callback callback) {
    class DefaultFileRequest : public AsyncRequest {
    public:
        DefaultFileRequest(Resource resource_, FileSource::Callback callback_, util::Thread<DefaultFileSource::Impl>& thread_)
            : thread(thread_),
              workRequest(thread.invokeWithCallback(&DefaultFileSource::Impl::request, this, resource_, callback_)) {
        }

        ~DefaultFileRequest() override {
            thread.invoke(&DefaultFileSource::Impl::cancel, this);
        }

        util::Thread<DefaultFileSource::Impl>& thread;
        std::unique_ptr<AsyncRequest> workRequest;
    };

    if (isAssetURL(resource.url)) {
        return assetFileSource->request(resource, callback);
    } else if (LocalFileSource::acceptsURL(resource.url)) {
        return localFileSource->request(resource, callback);
    } else {
        return std::make_unique<DefaultFileRequest>(resource, callback, *thread);
    }
}

void DefaultFileSource::listOfflineRegions(std::function<void (std::exception_ptr, optional<std::vector<OfflineRegion>>)> callback) {
    thread->invoke(&Impl::listRegions, callback);
}

void DefaultFileSource::createOfflineRegion(const OfflineRegionDefinition& definition,
                                            const OfflineRegionMetadata& metadata,
                                            std::function<void (std::exception_ptr, optional<OfflineRegion>)> callback) {
    thread->invoke(&Impl::createRegion, definition, metadata, callback);
}

void DefaultFileSource::updateOfflineMetadata(const int64_t regionID,
                                            const OfflineRegionMetadata& metadata,
                                            std::function<void (std::exception_ptr, optional<OfflineRegionMetadata>)> callback) {
    thread->invoke(&Impl::updateMetadata, regionID, metadata, callback);
}

void DefaultFileSource::deleteOfflineRegion(OfflineRegion&& region, std::function<void (std::exception_ptr)> callback) {
    thread->invoke(&Impl::deleteRegion, std::move(region), callback);
}

void DefaultFileSource::setOfflineRegionObserver(OfflineRegion& region, std::unique_ptr<OfflineRegionObserver> observer) {
    thread->invoke(&Impl::setRegionObserver, region.getID(), std::move(observer));
}

void DefaultFileSource::setOfflineRegionDownloadState(OfflineRegion& region, OfflineRegionDownloadState state) {
    thread->invoke(&Impl::setRegionDownloadState, region.getID(), state);
}

void DefaultFileSource::getOfflineRegionStatus(OfflineRegion& region, std::function<void (std::exception_ptr, optional<OfflineRegionStatus>)> callback) const {
    thread->invoke(&Impl::getRegionStatus, region.getID(), callback);
}

void DefaultFileSource::setOfflineMapboxTileCountLimit(uint64_t limit) const {
    thread->invokeSync(&Impl::setOfflineMapboxTileCountLimit, limit);
}

// For testing only:

void DefaultFileSource::put(const Resource& resource, const Response& response) {
    thread->invokeSync(&Impl::put, resource, response);
}

} // namespace mbgl
