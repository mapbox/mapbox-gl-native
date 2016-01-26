#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/offline_database.hpp>

#include <mbgl/platform/platform.hpp>
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
    class Task {
    public:
        Task(Resource resource, FileSource::Callback callback, DefaultFileSource::Impl* impl) {
            auto offlineResponse = impl->offlineDatabase.get(resource);

            Resource revalidation = resource;

            if (offlineResponse) {
                revalidation.priorModified = offlineResponse->modified;
                revalidation.priorExpires = offlineResponse->expires;
                revalidation.priorEtag = offlineResponse->etag;
                callback(*offlineResponse);
            }

            if (!impl->offline) {
                onlineRequest = impl->onlineFileSource.request(revalidation, [=] (Response onlineResponse) {
                    impl->offlineDatabase.put(revalidation, onlineResponse);
                    callback(onlineResponse);
                });
            }
        }

        std::unique_ptr<FileRequest> onlineRequest;
    };

    Impl(const std::string& cachePath)
        : offlineDatabase(cachePath) {
    }

    void setAccessToken(const std::string& accessToken) {
        onlineFileSource.setAccessToken(accessToken);
    }

    std::string getAccessToken() const {
        return onlineFileSource.getAccessToken();
    }

    void add(FileRequest* req, Resource resource, Callback callback) {
        tasks[req] = std::make_unique<Task>(resource, callback, this);
    }

    void cancel(FileRequest* req) {
        tasks.erase(req);
    }

    void put(const Resource& resource, const Response& response) {
        offlineDatabase.put(resource, response);
    }

    void goOffline() {
        offline = true;
    }

    OfflineDatabase offlineDatabase;
    OnlineFileSource onlineFileSource;
    std::unordered_map<FileRequest*, std::unique_ptr<Task>> tasks;
    bool offline = false;
};

class DefaultFileRequest : public FileRequest {
public:
    DefaultFileRequest(Resource     resource, FileSource::Callback callback, util::Thread<DefaultFileSource::Impl>& thread_)
        : thread(thread_),
          workRequest(thread.invokeWithCallback(&DefaultFileSource::Impl::add, callback, this, resource)) {
    }

    ~DefaultFileRequest() {
        thread.invoke(&DefaultFileSource::Impl::cancel, this);
    }

    util::Thread<DefaultFileSource::Impl>& thread;
    std::unique_ptr<WorkRequest> workRequest;
};

DefaultFileSource::DefaultFileSource(const std::string& cachePath, const std::string& assetRoot)
    : thread(std::make_unique<util::Thread<DefaultFileSource::Impl>>(util::ThreadContext{"DefaultFileSource", util::ThreadType::Unknown, util::ThreadPriority::Low}, cachePath)),
      assetFileSource(std::make_unique<AssetFileSource>(assetRoot)) {
}

DefaultFileSource::~DefaultFileSource() = default;

void DefaultFileSource::setAccessToken(const std::string& accessToken) {
    thread->invokeSync(&Impl::setAccessToken, accessToken);
}

std::string DefaultFileSource::getAccessToken() const {
    return thread->invokeSync<std::string>(&Impl::getAccessToken);
}

void DefaultFileSource::setMaximumCacheSize(uint64_t) {
    // TODO
}

void DefaultFileSource::setMaximumCacheEntrySize(uint64_t) {
    // TODO
}

std::unique_ptr<FileRequest> DefaultFileSource::request(const Resource& resource, Callback callback) {
    if (isAssetURL(resource.url)) {
        return assetFileSource->request(resource, callback);
    } else {
        return std::make_unique<DefaultFileRequest>(resource, callback, *thread);
    }
}

void DefaultFileSource::put(const Resource& resource, const Response& response) {
    thread->invokeSync(&Impl::put, resource, response);
}

void DefaultFileSource::goOffline() {
    thread->invokeSync(&Impl::goOffline);
}

} // namespace mbgl
