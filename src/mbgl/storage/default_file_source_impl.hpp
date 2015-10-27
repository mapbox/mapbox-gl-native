#ifndef MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE_IMPL
#define MBGL_STORAGE_DEFAULT_DEFAULT_FILE_SOURCE_IMPL

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/asset_context_base.hpp>
#include <mbgl/storage/http_context_base.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/timer.hpp>

#include <set>
#include <unordered_map>

namespace mbgl {

class RequestBase;

class DefaultFileRequest : public FileRequest {
public:
    DefaultFileRequest(const Resource& resource_,
                       DefaultFileSource& fileSource_)
        : resource(resource_),
          fileSource(fileSource_) {
    }

    ~DefaultFileRequest() {
        fileSource.cancel(resource, this);
    }

    Resource resource;
    DefaultFileSource& fileSource;

    std::unique_ptr<WorkRequest> workRequest;
};

class DefaultFileRequestImpl : public util::noncopyable {
public:
    using Callback = std::function<void (Response)>;

    const Resource resource;
    std::unique_ptr<WorkRequest> cacheRequest;
    RequestBase* realRequest = nullptr;
    std::unique_ptr<util::Timer> timerRequest;

    inline DefaultFileRequestImpl(const Resource& resource_)
        : resource(resource_) {}

    ~DefaultFileRequestImpl();

    // Observer accessors.
    void addObserver(FileRequest*, Callback);
    void removeObserver(FileRequest*);
    bool hasObservers() const;

    // Updates/gets the response of this request object.
    void setResponse(const std::shared_ptr<const Response>&);
    const std::shared_ptr<const Response>& getResponse() const;

    // Returns the seconds we have to wait until we need to redo this request. A value of 0
    // means that we need to redo it immediately, and a negative value means that we're not setting
    // a timeout at all.
    Seconds getRetryTimeout() const;

    // Checks the currently stored response and replaces it with an idential one, except with the
    // stale flag set, if the response is expired.
    void checkResponseFreshness();

    // Notifies all observers.
    void notify();


private:
    // Stores a set of all observing Request objects.
    std::unordered_map<FileRequest*, Callback> observers;

    // The current response data. We're storing it because we can satisfy requests for the same
    // resource directly by returning this response object. We also need it to create conditional
    // HTTP requests, and to check whether new responses we got changed any data.
    std::shared_ptr<const Response> response;

    // Counts the number of subsequent failed requests. We're using this value for exponential
    // backoff when retrying requests.
    int failedRequests = 0;
};

class DefaultFileSource::Impl {
public:
    using Callback = std::function<void (Response)>;

    Impl(FileCache*, const std::string& = "");
    ~Impl();

    void networkIsReachableAgain();

    void add(Resource, FileRequest*, Callback);
    void cancel(Resource, FileRequest*);

private:
    void update(DefaultFileRequestImpl&);
    void startCacheRequest(DefaultFileRequestImpl&);
    void startRealRequest(DefaultFileRequestImpl&);
    void reschedule(DefaultFileRequestImpl&);

    std::unordered_map<Resource, std::unique_ptr<DefaultFileRequestImpl>, Resource::Hash> pending;
    FileCache* const cache;
    const std::string assetRoot;
    const std::unique_ptr<AssetContextBase> assetContext;
    const std::unique_ptr<HTTPContextBase> httpContext;
    util::AsyncTask reachability;
};

}

#endif
