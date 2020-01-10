#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/storage/file_source_request.hpp>
#include <mbgl/storage/main_resource_loader.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/util/stopwatch.hpp>

#include <cassert>

namespace mbgl {

class ResourceLoaderRequestor {
public:
    explicit ResourceLoaderRequestor(MainResourceLoader::Impl& impl_);
    void request(AsyncRequest*, Resource, ActorRef<FileSourceRequest>);
    void cancel(AsyncRequest*);
    void pause();
    void resume();

private:
    MainResourceLoader::Impl& impl;
};

class MainResourceLoader::Impl {
public:
    Impl(std::shared_ptr<FileSource> assetFileSource_,
         std::shared_ptr<FileSource> databaseFileSource_,
         std::shared_ptr<FileSource> localFileSource_,
         std::shared_ptr<FileSource> onlineFileSource_)
        : assetFileSource(std::move(assetFileSource_)),
          databaseFileSource(std::move(databaseFileSource_)),
          localFileSource(std::move(localFileSource_)),
          onlineFileSource(std::move(onlineFileSource_)),
          supportsCacheOnlyRequests_(bool(databaseFileSource)),
          requestor(std::make_unique<Actor<ResourceLoaderRequestor>>(*Scheduler::GetCurrent(), *this)) {}

    std::unique_ptr<AsyncRequest> request(const Resource& resource, Callback callback) {
        auto req = std::make_unique<FileSourceRequest>(std::move(callback));
        req->onCancel([actorRef = requestor->self(), req = req.get()]() {
            actorRef.invoke(&ResourceLoaderRequestor::cancel, req);
        });
        requestor->self().invoke(&ResourceLoaderRequestor::request, req.get(), resource, req->actor());
        return std::move(req);
    }

    bool canRequest(const Resource& resource) const {
        return (assetFileSource && assetFileSource->canRequest(resource)) ||
               (localFileSource && localFileSource->canRequest(resource)) ||
               (databaseFileSource && databaseFileSource->canRequest(resource)) ||
               (onlineFileSource && onlineFileSource->canRequest(resource));
    }

    bool supportsCacheOnlyRequests() const { return supportsCacheOnlyRequests_; }

    void pause() { requestor->self().invoke(&ResourceLoaderRequestor::pause); }

    void resume() { requestor->self().invoke(&ResourceLoaderRequestor::resume); }

private:
    void request(AsyncRequest* req, Resource resource, ActorRef<FileSourceRequest> ref) {
        auto callback = [ref](const Response& res) { ref.invoke(&FileSourceRequest::setResponse, res); };

        auto requestFromNetwork = [=](const Resource& res,
                                      std::unique_ptr<AsyncRequest> parent) -> std::unique_ptr<AsyncRequest> {
            if (!onlineFileSource || !onlineFileSource->canRequest(resource)) {
                return parent;
            }

            // Keep parent request alive while chained request is being processed.
            std::shared_ptr<AsyncRequest> parentKeepAlive = std::move(parent);

            MBGL_TIMING_START(watch);
            return onlineFileSource->request(res, [=, ptr = parentKeepAlive](Response response) {
                if (databaseFileSource) {
                    databaseFileSource->forward(res, response);
                }
                if (res.kind == Resource::Kind::Tile) {
                    // onlineResponse.data will be null if data not modified
                    MBGL_TIMING_FINISH(watch,
                                       " Action: "
                                           << "Requesting,"
                                           << " URL: " << res.url.c_str() << " Size: "
                                           << (response.data != nullptr ? response.data->size() : 0) << "B,"
                                           << " Time")
                }
                callback(response);
            });
        };

        // Initial tasksSize is used to check whether any of
        // the sources were able to request a resource.
        const std::size_t tasksSize = tasks.size();

        // Waterfall resource request processing and return early once resource was requested.
        if (assetFileSource && assetFileSource->canRequest(resource)) {
            // Asset request
            tasks[req] = assetFileSource->request(resource, callback);
        } else if (localFileSource && localFileSource->canRequest(resource)) {
            // Local file request
            tasks[req] = localFileSource->request(resource, callback);
        } else if (databaseFileSource && databaseFileSource->canRequest(resource)) {
            // Try cache only request if needed.
            if (resource.loadingMethod == Resource::LoadingMethod::CacheOnly) {
                tasks[req] = databaseFileSource->request(resource, callback);
            } else {
                // Cache request with fallback to network with cache control
                tasks[req] = databaseFileSource->request(resource, [=](Response response) {
                    Resource res = resource;

                    // Resource is in the cache
                    if (!response.noContent) {
                        if (response.isUsable()) {
                            callback(response);
                            // Set the priority of existing resource to low if it's expired but usable.
                            res.setPriority(Resource::Priority::Low);
                        }

                        // Copy response fields for cache control request
                        res.priorModified = response.modified;
                        res.priorExpires = response.expires;
                        res.priorEtag = response.etag;
                        res.priorData = response.data;
                    }

                    tasks[req] = requestFromNetwork(res, std::move(tasks[req]));
                });
            }
        } else if (auto networkReq = requestFromNetwork(resource, nullptr)) {
            // Get from the online file source
            tasks[req] = std::move(networkReq);
        }

        // If no new tasks were added, notify client that request cannot be processed.
        if (tasks.size() == tasksSize) {
            Response response;
            response.noContent = true;
            response.error =
                std::make_unique<Response::Error>(Response::Error::Reason::Other, "Unsupported resource request.");
            callback(response);
        }
    }

    void pauseInternal() {
        if (assetFileSource) assetFileSource->pause();
        if (databaseFileSource) databaseFileSource->pause();
        if (localFileSource) localFileSource->pause();
        if (onlineFileSource) onlineFileSource->pause();
    }

    void resumeInternal() {
        if (assetFileSource) assetFileSource->resume();
        if (databaseFileSource) databaseFileSource->resume();
        if (localFileSource) localFileSource->resume();
        if (onlineFileSource) onlineFileSource->resume();
    }

    void cancel(AsyncRequest* req) {
        assert(req);
        tasks.erase(req);
    }

private:
    friend class ResourceLoaderRequestor;
    const std::shared_ptr<FileSource> assetFileSource;
    const std::shared_ptr<FileSource> databaseFileSource;
    const std::shared_ptr<FileSource> localFileSource;
    const std::shared_ptr<FileSource> onlineFileSource;
    const bool supportsCacheOnlyRequests_;
    std::unique_ptr<Actor<ResourceLoaderRequestor>> requestor;
    std::unordered_map<AsyncRequest*, std::unique_ptr<AsyncRequest>> tasks;
};

ResourceLoaderRequestor::ResourceLoaderRequestor(MainResourceLoader::Impl& impl_) : impl(impl_) {}

void ResourceLoaderRequestor::request(AsyncRequest* req, Resource resource, ActorRef<FileSourceRequest> ref) {
    assert(req);
    impl.request(req, std::move(resource), std::move(ref));
}

void ResourceLoaderRequestor::cancel(AsyncRequest* req) {
    assert(req);
    impl.cancel(req);
}

void ResourceLoaderRequestor::pause() {
    impl.pauseInternal();
}

void ResourceLoaderRequestor::resume() {
    impl.resumeInternal();
}

MainResourceLoader::MainResourceLoader(const ResourceOptions& options)
    : impl(std::make_unique<Impl>(FileSourceManager::get()->getFileSource(FileSourceType::Asset, options),
                                  FileSourceManager::get()->getFileSource(FileSourceType::Database, options),
                                  FileSourceManager::get()->getFileSource(FileSourceType::FileSystem, options),
                                  FileSourceManager::get()->getFileSource(FileSourceType::Network, options))) {}

MainResourceLoader::~MainResourceLoader() = default;

bool MainResourceLoader::supportsCacheOnlyRequests() const {
    return impl->supportsCacheOnlyRequests();
}

std::unique_ptr<AsyncRequest> MainResourceLoader::request(const Resource& resource, Callback callback) {
    return impl->request(resource, std::move(callback));
}

bool MainResourceLoader::canRequest(const Resource& resource) const {
    return impl->canRequest(resource);
}

void MainResourceLoader::pause() {
    impl->pause();
}

void MainResourceLoader::resume() {
    impl->resume();
}

} // namespace mbgl
