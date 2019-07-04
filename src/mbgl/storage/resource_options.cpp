#include <mbgl/storage/resource_options.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

class ResourceOptions::Impl {
public:
    std::string accessToken;
    std::string baseURL = mbgl::util::API_BASE_URL;
    std::string cachePath = ":memory:";
    std::string assetPath = ".";
    uint64_t maximumSize = mbgl::util::DEFAULT_MAX_CACHE_SIZE;
    bool supportCacheOnlyRequests = true;
    void* platformContext = nullptr;
};

// These requires the complete type of Impl.
ResourceOptions::ResourceOptions() : impl_(std::make_unique<Impl>()) {}
ResourceOptions::~ResourceOptions() = default;
ResourceOptions::ResourceOptions(ResourceOptions&&) noexcept = default;
ResourceOptions::ResourceOptions(const ResourceOptions& other) : impl_(std::make_unique<Impl>(*other.impl_)) {}

ResourceOptions ResourceOptions::clone() const {
    return ResourceOptions(*this);
}

ResourceOptions& ResourceOptions::withAccessToken(std::string token) {
    impl_->accessToken = std::move(token);
    return *this;
}

const std::string& ResourceOptions::accessToken() const {
    return impl_->accessToken;
}

ResourceOptions& ResourceOptions::withBaseURL(std::string url) {
    impl_->baseURL = std::move(url);
    return *this;
}

const std::string& ResourceOptions::baseURL() const {
    return impl_->baseURL;
}

ResourceOptions& ResourceOptions::withCachePath(std::string path) {
    impl_->cachePath = std::move(path);
    return *this;
}

const std::string& ResourceOptions::cachePath() const {
    return impl_->cachePath;
}

ResourceOptions& ResourceOptions::withAssetPath(std::string path) {
    impl_->assetPath = std::move(path);
    return *this;
}

const std::string& ResourceOptions::assetPath() const {
    return impl_->assetPath;
}

ResourceOptions& ResourceOptions::withMaximumCacheSize(uint64_t size) {
    impl_->maximumSize = size;
    return *this;
}

uint64_t ResourceOptions::maximumCacheSize() const {
    return impl_->maximumSize;
}

ResourceOptions& ResourceOptions::withCacheOnlyRequestsSupport(bool supportCacheOnlyRequests) {
    impl_->supportCacheOnlyRequests = supportCacheOnlyRequests;
    return *this;
}

bool ResourceOptions::supportsCacheOnlyRequests() const {
    return impl_->supportCacheOnlyRequests;
}

ResourceOptions& ResourceOptions::withPlatformContext(void* context) {
    impl_->platformContext = context;
    return *this;
}

void* ResourceOptions::platformContext() const {
    return impl_->platformContext;
}

}  // namespace mbgl
