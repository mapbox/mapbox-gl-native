#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/util.hpp>

#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/actor_ref.hpp>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace mbgl {

class AssetFileRequest : public AsyncRequest {
public:
    AssetFileRequest(std::shared_ptr<FileSource::Callback> callback_)
        : callback(std::move(callback_)) {
    }

private:
    std::shared_ptr<FileSource::Callback> callback;
};

class AssetFileSource::Worker {
public:
    Worker(ActorRef<Worker>, ActorRef<AssetFileSource> parent_, std::string root_)
        : parent(std::move(parent_)), root(std::move(root_)) {
    }
    void readFile(const std::string& url, std::weak_ptr<Callback> callback) {
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

        if (result == 0 && S_ISDIR(buf.st_mode)) {
            response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);
        } else if (result == -1 && errno == ENOENT) {
            response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);
        } else {
            try {
                response.data = std::make_shared<std::string>(util::read_file(path));
            } catch (...) {
                response.error = std::make_unique<Response::Error>(
                    Response::Error::Reason::Other, util::toString(std::current_exception()));
            }
        }

        parent.invoke(&AssetFileSource::respond, callback, response);
    }

private:
    ActorRef<AssetFileSource> parent;
    const std::string root;
};

AssetFileSource::AssetFileSource(Scheduler& scheduler, const std::string& root)
    : mailbox(std::make_shared<Mailbox>(*util::RunLoop::Get())),
      worker(std::make_unique<Actor<Worker>>(
          scheduler, ActorRef<AssetFileSource>(*this, mailbox), root)) {
}

AssetFileSource::~AssetFileSource() = default;

std::unique_ptr<AsyncRequest> AssetFileSource::request(const Resource& resource,
                                                       Callback callback) {
    auto cb = std::make_shared<Callback>(std::move(callback));
    worker->invoke(&Worker::readFile, resource.url, cb);
    return std::make_unique<AssetFileRequest>(std::move(cb));
}

void AssetFileSource::respond(std::weak_ptr<Callback> callback, Response response) {
    if (auto locked = callback.lock()) {
        (*locked)(response);
    }
}

} // namespace mbgl
