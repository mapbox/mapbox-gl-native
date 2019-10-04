#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/util/string.hpp>

#include <map>
#include <mutex>
#include <tuple>

namespace mbgl {

class FileSourceManager::Impl {
public:
    using Key = std::tuple<FileSourceType, std::string>;
    std::map<Key, std::weak_ptr<FileSource>> fileSources;
    std::map<FileSourceType, FileSourceFactory> fileSourceFactories;
    std::recursive_mutex mutex;
};

FileSourceManager::FileSourceManager() : impl(std::make_unique<Impl>()) {}

FileSourceManager::~FileSourceManager() = default;

std::shared_ptr<FileSource> FileSourceManager::getFileSource(FileSourceType type,
                                                             const ResourceOptions& options) noexcept {
    std::lock_guard<std::recursive_mutex> lock(impl->mutex);

    // Remove released file sources.
    for (auto it = impl->fileSources.begin(); it != impl->fileSources.end();) {
        it = it->second.expired() ? impl->fileSources.erase(it) : ++it;
    }

    const auto context = reinterpret_cast<uint64_t>(options.platformContext());
    const std::string optionsKey =
        options.baseURL() + '|' + options.accessToken() + '|' + options.cachePath() + '|' + util::toString(context);
    const auto key = std::tie(type, optionsKey);

    std::shared_ptr<FileSource> fileSource;
    auto tuple = impl->fileSources.find(key);
    if (tuple != impl->fileSources.end()) {
        fileSource = tuple->second.lock();
    }

    if (!fileSource) {
        auto it = impl->fileSourceFactories.find(type);
        if (it != impl->fileSourceFactories.end()) {
            assert(it->second);
            impl->fileSources[key] = fileSource = it->second(options);
        }
    }

    return fileSource;
}

void FileSourceManager::registerFileSourceFactory(FileSourceType type, FileSourceFactory&& factory) noexcept {
    assert(factory);
    std::lock_guard<std::recursive_mutex> lock(impl->mutex);
    impl->fileSourceFactories[type] = std::move(factory);
}

FileSourceManager::FileSourceFactory FileSourceManager::unRegisterFileSourceFactory(FileSourceType type) noexcept {
    std::lock_guard<std::recursive_mutex> lock(impl->mutex);
    auto it = impl->fileSourceFactories.find(type);
    FileSourceFactory factory;
    if (it != impl->fileSourceFactories.end()) {
        factory = std::move(it->second);
        impl->fileSourceFactories.erase(it);
    }
    return factory;
}

} // namespace mbgl
