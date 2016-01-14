#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

#include <mbgl/platform/platform.hpp>
#include <mbgl/util/url.hpp>

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
          cache(SQLiteCache::getShared(cachePath)),
          onlineFileSource(cache.get()) {
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

std::unique_ptr<FileRequest> DefaultFileSource::request(const Resource& resource, Callback callback) {
    if (isAssetURL(resource.url)) {
        return impl->assetFileSource.request(resource, callback);
    } else {
        return impl->onlineFileSource.request(resource, callback);
    }
}

} // namespace mbgl
