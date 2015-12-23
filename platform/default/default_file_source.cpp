#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>

namespace mbgl {

class DefaultFileSource::Impl {
public:
    Impl(FileCache* cache, const std::string& root)
        : onlineFileSource(cache, root) {
    }

    OnlineFileSource onlineFileSource;
};

DefaultFileSource::DefaultFileSource(FileCache* cache, const std::string& root)
    : impl(std::make_unique<DefaultFileSource::Impl>(cache, root)) {
}

DefaultFileSource::~DefaultFileSource() = default;

void DefaultFileSource::setAccessToken(const std::string& accessToken) {
    impl->onlineFileSource.setAccessToken(accessToken);
}

std::string DefaultFileSource::getAccessToken() const {
    return impl->onlineFileSource.getAccessToken();
}

std::unique_ptr<FileRequest> DefaultFileSource::request(const Resource& resource, Callback callback) {
    return impl->onlineFileSource.request(resource, callback);
}

} // namespace mbgl
