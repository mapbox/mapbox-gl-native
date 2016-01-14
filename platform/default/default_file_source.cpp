#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

#include <mbgl/platform/platform.hpp>
#include <mbgl/util/url.hpp>
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
    Impl(const std::string& cachePath, const std::string& assetRoot)
        : assetFileSource(assetRoot),
          cache(SQLiteCache::getShared(cachePath)) {
    }

    AssetFileSource assetFileSource;
    std::shared_ptr<SQLiteCache> cache;
    OnlineFileSource onlineFileSource;
};

DefaultFileSource::DefaultFileSource(const std::string& cachePath, const std::string& assetRoot)
    : impl(std::make_unique<DefaultFileSource::Impl>(cachePath, assetRoot)) {
}

DefaultFileSource::~DefaultFileSource() = default;

void DefaultFileSource::setAccessToken(const std::string& accessToken) {
    impl->onlineFileSource.setAccessToken(accessToken);
}

std::string DefaultFileSource::getAccessToken() const {
    return impl->onlineFileSource.getAccessToken();
}

void DefaultFileSource::setMaximumCacheSize(uint64_t size) {
    impl->cache->setMaximumCacheSize(size);
}

void DefaultFileSource::setMaximumCacheEntrySize(uint64_t size) {
    impl->cache->setMaximumCacheEntrySize(size);
}

SQLiteCache& DefaultFileSource::getCache() {
    return *impl->cache;
}

class DefaultFileRequest : public FileRequest {
public:
    DefaultFileRequest(Resource resource, FileSource::Callback callback, DefaultFileSource::Impl* impl) {
        cacheRequest = impl->cache->get(resource, [=](std::shared_ptr<Response> cacheResponse) mutable {
            cacheRequest.reset();

            if (cacheResponse) {
                resource.priorModified = cacheResponse->modified;
                resource.priorExpires = cacheResponse->expires;
                resource.priorEtag = cacheResponse->etag;
            }

            onlineRequest = impl->onlineFileSource.request(resource, [=] (Response onlineResponse) {
                impl->cache->put(resource, onlineResponse);
                callback(onlineResponse);
            });

            // Do this last because it may result in deleting this DefaultFileRequest.
            if (cacheResponse) {
                callback(*cacheResponse);
            }
        });
    }

    std::unique_ptr<WorkRequest> cacheRequest;
    std::unique_ptr<FileRequest> onlineRequest;
};

std::unique_ptr<FileRequest> DefaultFileSource::request(const Resource& resource, Callback callback) {
    if (isAssetURL(resource.url)) {
        return impl->assetFileSource.request(resource, callback);
    } else {
        return std::make_unique<DefaultFileRequest>(resource, callback, impl.get());
    }
}

} // namespace mbgl
