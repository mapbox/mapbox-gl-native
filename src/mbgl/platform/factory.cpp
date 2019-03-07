#include <mbgl/platform/factory.hpp>
#include <mbgl/storage/default_file_source.hpp>

namespace mbgl {
namespace platform {

std::shared_ptr<FileSource> Factory::sharedFileSource(const FileSourceOptions& options, std::shared_ptr<FileSource> initial) {
    static std::weak_ptr<FileSource> weak;

    if (initial) {
        // We can only assign a custom initial value if the weak pointer object has zero references.
        assert(weak.use_count() == 0);
        weak = initial;
    }

    std::shared_ptr<FileSource> strong = weak.lock();
    if (!strong) {
        auto defaultFileSource = std::make_shared<DefaultFileSource>(options.cachePath(), options.assetRoot(), options.maximumCacheSize());
        defaultFileSource->setAccessToken(options.accessToken());
        weak = strong = defaultFileSource;
    }

    return strong;
}

} // namespace platform
} // namespace mbgl
