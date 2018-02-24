#include <mbgl/storage/local_file_source.hpp>
#include <mbgl/storage/file_source_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/io.hpp>

#include <sys/types.h>
#include <sys/stat.h>

#if defined(_WINDOWS) && !defined(S_ISDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

namespace {

const std::string fileProtocol = "file://";

} // namespace

namespace mbgl {

class LocalFileSource::Impl {
public:
    Impl(ActorRef<Impl>) {}

    void request(const std::string& url, ActorRef<FileSourceRequest> req) {
        Response response;

        if (!acceptsURL(url)) {
            response.error = std::make_unique<Response::Error>(Response::Error::Reason::Other,
                                                               "Invalid file URL");
            req.invoke(&FileSourceRequest::setResponse, response);
            return;
        }

        // Cut off the protocol and prefix with path.
        const auto path = mbgl::util::percentDecode(url.substr(fileProtocol.size()));
        struct stat buf;
        int result = stat(path.c_str(), &buf);

        if (result == 0 && S_ISDIR(buf.st_mode)) {
            response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);
        } else if (result == -1 && errno == ENOENT) {
            response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);
        } else {
            try {
                response.data = std::make_shared<std::string>(util::read_file(path));
            } catch (...) {
                response.error = std::make_unique<Response::Error>(
                    Response::Error::Reason::Other,
                    util::toString(std::current_exception()));
            }
        }

        req.invoke(&FileSourceRequest::setResponse, response);
    }

};

LocalFileSource::LocalFileSource()
    : impl(std::make_unique<util::Thread<Impl>>("LocalFileSource")) {
}

LocalFileSource::~LocalFileSource() = default;

std::unique_ptr<AsyncRequest> LocalFileSource::request(const Resource& resource, Callback callback) {
    auto req = std::make_unique<FileSourceRequest>(std::move(callback));

    impl->actor().invoke(&Impl::request, resource.url, req->actor());

    return std::move(req);
}

bool LocalFileSource::acceptsURL(const std::string& url) {
    return std::equal(fileProtocol.begin(), fileProtocol.end(), url.begin());
}

} // namespace mbgl
