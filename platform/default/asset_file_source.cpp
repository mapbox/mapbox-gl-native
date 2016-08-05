#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/io.hpp>

#include <sys/types.h>
#include <sys/stat.h>

namespace mbgl {

class AssetFileSource::Impl {
public:
    Impl(std::string root_)
        : root(std::move(root_)) {
    }

    void request(const std::string& url, FileSource::Callback callback) {
        std::string path;

        if (url.size() <= 8 || url[8] == '/') {
            // This is an empty or absolute path.
            path = mbgl::util::percentDecode(url.substr(8));
        } else {
            // This is a relative path. Prefix with the application root.
            path = root + "/" + mbgl::util::percentDecode(url.substr(8));
        }

        Response response;

        struct stat buf;
        int result = stat(path.c_str(), &buf);

        if (result == 0 && (S_IFDIR & buf.st_mode)) {
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

private:
    std::string root;
};

AssetFileSource::AssetFileSource(const std::string& root)
    : thread(std::make_unique<util::Thread<Impl>>(
        util::ThreadContext{"AssetFileSource", util::ThreadPriority::Low},
        root)) {
}

AssetFileSource::~AssetFileSource() = default;

std::unique_ptr<AsyncRequest> AssetFileSource::request(const Resource& resource, Callback callback) {
    return thread->invokeWithCallback(&Impl::request, resource.url, callback);
}

} // namespace mbgl
