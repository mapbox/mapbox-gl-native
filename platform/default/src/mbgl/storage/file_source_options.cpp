#include <mbgl/storage/file_source_options.hpp>
#include <mbgl/util/constants.hpp>

#include <cassert>

namespace mbgl {

class FileSourceOptions::Impl {
public:
    std::string accessToken;
    std::string cachePath { ":memory:" };
    std::string assetRoot { "." };
    uint64_t maximumSize{mbgl::util::DEFAULT_MAX_CACHE_SIZE};
};

FileSourceOptions::FileSourceOptions() : impl_(std::make_shared<FileSourceOptions::Impl>()) {}
FileSourceOptions::~FileSourceOptions() = default;

FileSourceOptions& FileSourceOptions::withAccessToken(std::string accessToken) {
    impl_->accessToken = std::move(accessToken);
    return *this;
}

const std::string& FileSourceOptions::accessToken() const {
    return impl_->accessToken;
}

FileSourceOptions& FileSourceOptions::withCachePath(std::string path) {
    impl_->cachePath = std::move(path);
    return *this;
}

const std::string& FileSourceOptions::cachePath() const {
    return impl_->cachePath;
}

FileSourceOptions& FileSourceOptions::withAssetRoot(std::string path) {
    impl_->assetRoot = std::move(path);
    return *this;
}

const std::string& FileSourceOptions::assetRoot() const {
    return impl_->assetRoot;
}

FileSourceOptions& FileSourceOptions::withMaximumCacheSize(uint64_t size) {
    impl_->maximumSize = size;
    return *this;
}

uint64_t FileSourceOptions::maximumCacheSize() const {
    return impl_->maximumSize;
}

} // namespace mbgl