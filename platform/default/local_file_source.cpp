#include <mbgl/storage/local_file_source.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/io.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace {

const char* protocol = "file://";
const std::size_t protocolLength = 7;

} // namespace

namespace mbgl {

class LocalFileSource::Impl {
public:
    void request(const std::string& url, FileSource::Callback callback) {
        // Cut off the protocol
        std::string path = mbgl::util::percentDecode(url.substr(protocolLength));

        Response response;

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

        callback(response);
    }

};

LocalFileSource::LocalFileSource()
    : thread(std::make_unique<util::Thread<Impl>>(util::ThreadContext{"LocalFileSource", util::ThreadPriority::Low})) {
}

LocalFileSource::~LocalFileSource() = default;

std::unique_ptr<AsyncRequest> LocalFileSource::request(const Resource& resource, Callback callback) {
    return thread->invokeWithCallback(&Impl::request, resource.url, callback);
}

bool LocalFileSource::acceptsURL(const std::string& url) {
    return url.compare(0, protocolLength, protocol) == 0;
}

} // namespace mbgl
