#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

namespace mbgl {

class DefaultFileSource::Impl {
public:
    Impl(const std::string& cachePath, const std::string& root)
        : cache(SharedSQLiteCache::get(cachePath)),
          onlineFileSource(cache.get(), root) {
    }

    std::shared_ptr<SQLiteCache> cache;
    OnlineFileSource onlineFileSource;
};

DefaultFileSource::DefaultFileSource(const std::string& cachePath, const std::string& root)
    : impl(std::make_unique<DefaultFileSource::Impl>(cachePath, root)) {
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
    return impl->onlineFileSource.request(resource, callback);
}

} // namespace mbgl
