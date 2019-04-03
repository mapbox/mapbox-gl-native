#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/util/string.hpp>

#include <mutex>
#include <map>

namespace mbgl {

std::shared_ptr<FileSource> FileSource::getSharedFileSource(const ResourceOptions& options) {
    static std::mutex mutex;
    static std::map<std::string, std::weak_ptr<mbgl::FileSource>> fileSources;

    std::lock_guard<std::mutex> lock(mutex);

    // Purge entries no longer in use.
    for (auto it = fileSources.begin(); it != fileSources.end();) {
        it = it->second.expired() ? fileSources.erase(it) : ++it;
    }

    const auto context = reinterpret_cast<uint64_t>(options.platformContext());
    const std::string key = options.baseURL() + '|' + options.accessToken() + '|' + options.cachePath() + '|' + util::toString(context);

    std::shared_ptr<mbgl::FileSource> fileSource;
    auto tuple = fileSources.find(key);
    if (tuple != fileSources.end()) {
        fileSource = tuple->second.lock();
    }

    if (!fileSource) {
        fileSources[key] = fileSource = createPlatformFileSource(options);
    }

    return fileSource;
}

} // namespace mbgl
