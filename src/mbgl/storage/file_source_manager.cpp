#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/util/string.hpp>

#include <algorithm>
#include <list>
#include <map>
#include <mutex>
#include <tuple>

namespace mbgl {

struct FileSourceInfo {
    FileSourceInfo(FileSourceType type_, std::string id_, std::weak_ptr<FileSource> fileSource_)
        : type(type_), id(std::move(id_)), fileSource(std::move(fileSource_)) {}

    FileSourceType type;
    std::string id;
    std::weak_ptr<FileSource> fileSource;
};

class FileSourceManager::Impl {
public:
    std::list<FileSourceInfo> fileSources;
    std::map<FileSourceType, FileSourceFactory> fileSourceFactories;
    std::recursive_mutex mutex;
};

FileSourceManager::FileSourceManager() : impl(std::make_unique<Impl>()) {}

FileSourceManager::~FileSourceManager() = default;

PassRefPtr<FileSource> FileSourceManager::getFileSource(FileSourceType type, const ResourceOptions& options) noexcept {
    std::lock_guard<std::recursive_mutex> lock(impl->mutex);

    // Remove released file sources.
    for (auto it = impl->fileSources.begin(); it != impl->fileSources.end();) {
        it = it->fileSource.expired() ? impl->fileSources.erase(it) : ++it;
    }

    const auto context = reinterpret_cast<uint64_t>(options.platformContext());
    std::string id =
        options.baseURL() + '|' + options.accessToken() + '|' + options.cachePath() + '|' + util::toString(context);

    std::shared_ptr<FileSource> fileSource;
    auto fileSourceIt = std::find_if(impl->fileSources.begin(), impl->fileSources.end(), [type, &id](const auto& info) {
        return info.type == type && info.id == id;
    });
    if (fileSourceIt != impl->fileSources.end()) {
        fileSource = fileSourceIt->fileSource.lock();
    }

    if (!fileSource) {
        auto it = impl->fileSourceFactories.find(type);
        if (it != impl->fileSourceFactories.end()) {
            assert(it->second);
            fileSource = it->second(options);
            impl->fileSources.emplace_back(type, std::move(id), fileSource);
        }
    }

    return fileSource;
}

PassRefPtr<JointDatabaseStorage> FileSourceManager::getDatabaseStorage(const ResourceOptions&) noexcept {
    return {nullptr};
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
