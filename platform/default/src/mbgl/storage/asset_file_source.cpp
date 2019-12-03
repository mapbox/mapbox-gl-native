#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/file_source_request.hpp>
#include <mbgl/storage/local_file_request.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/url.hpp>

namespace {
bool acceptsURL(const std::string& url) {
    return 0 == url.rfind(mbgl::util::ASSET_PROTOCOL, 0);
}
} // namespace

namespace mbgl {

class AssetFileSource::Impl {
public:
    Impl(ActorRef<Impl>, std::string root_)
        : root(std::move(root_)) {
    }

    void request(const std::string& url, ActorRef<FileSourceRequest> req) {
        if (!acceptsURL(url)) {
            Response response;
            response.error = std::make_unique<Response::Error>(Response::Error::Reason::Other,
                                                               "Invalid asset URL");
            req.invoke(&FileSourceRequest::setResponse, response);
            return;
        }

        // Cut off the protocol and prefix with path.
        const auto path =
            root + "/" + mbgl::util::percentDecode(url.substr(std::char_traits<char>::length(util::ASSET_PROTOCOL)));
        requestLocalFile(path, std::move(req));
    }

private:
    std::string root;
};

AssetFileSource::AssetFileSource(const std::string& root)
    : impl(std::make_unique<util::Thread<Impl>>("AssetFileSource", root)) {
}

AssetFileSource::~AssetFileSource() = default;

std::unique_ptr<AsyncRequest> AssetFileSource::request(const Resource& resource, Callback callback) {
    auto req = std::make_unique<FileSourceRequest>(std::move(callback));

    impl->actor().invoke(&Impl::request, resource.url, req->actor());

    return req;
}

bool AssetFileSource::canRequest(const Resource& resource) const {
    return acceptsURL(resource.url);
}

void AssetFileSource::pause() {
    impl->pause();
}

void AssetFileSource::resume() {
    impl->resume();
}

} // namespace mbgl
