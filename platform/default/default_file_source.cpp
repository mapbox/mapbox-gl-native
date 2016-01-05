#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/offline_file_source.hpp>

namespace mbgl {

class DefaultFileSource::Impl {
public:
    Impl(FileCache* cache, const std::string& root, const std::string& offlinePath)
        : offlineFileSource(offlinePath),
          onlineFileSource(cache, root) {
    }

    OfflineFileSource offlineFileSource;
    OnlineFileSource onlineFileSource;
};

class DefaultFileRequest : public FileRequest {
public:
    DefaultFileRequest(const Resource& resource, FileSource::Callback callback, DefaultFileSource::Impl& impl) {
        offlineRequest = impl.offlineFileSource.request(resource, [&impl, resource, callback, this] (Response response) {
            if (response.error) {
                onlineRequest = impl.onlineFileSource.request(resource, callback);
            } else {
                callback(response);
            }
        });
    }

    std::unique_ptr<FileRequest> offlineRequest;
    std::unique_ptr<FileRequest> onlineRequest;
};

DefaultFileSource::DefaultFileSource(FileCache* cache, const std::string& root, const std::string& offlinePath)
    : impl(std::make_unique<DefaultFileSource::Impl>(cache, root, offlinePath)) {
}

DefaultFileSource::~DefaultFileSource() = default;

void DefaultFileSource::setAccessToken(const std::string& accessToken) {
    impl->onlineFileSource.setAccessToken(accessToken);
}

std::string DefaultFileSource::getAccessToken() const {
    return impl->onlineFileSource.getAccessToken();
}

std::unique_ptr<FileRequest> DefaultFileSource::request(const Resource& resource, Callback callback) {
    return std::make_unique<DefaultFileRequest>(resource, callback, *impl);
}

} // namespace mbgl
