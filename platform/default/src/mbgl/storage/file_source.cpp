#include <mbgl/storage/resource_options.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <memory>

namespace mbgl {

std::shared_ptr<FileSource> FileSource::createPlatformFileSource(const ResourceOptions& options) {
    auto fileSource = std::make_shared<DefaultFileSource>(options.cachePath(), options.assetPath(), options.supportsCacheOnlyRequests());
    fileSource->setAccessToken(options.accessToken());
    fileSource->setAPIBaseURL(options.baseURL());
    return fileSource;
}

} // namespace mbgl
