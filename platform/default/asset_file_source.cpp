#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/run_loop.hpp>

#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/actor_ref.hpp>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace mbgl {

class AssetFileSource::Impl {
public:
    Impl(Scheduler&, const std::string& root);

    class Request;
    std::unique_ptr<Request> readFile(const Resource&, Callback);
    void respond(std::weak_ptr<Callback>, Response);

private:
    std::shared_ptr<Mailbox> mailbox{ std::make_shared<Mailbox>(*util::RunLoop::Get()) };
    class Worker {
    public:
        Worker(ActorRef<Worker>, ActorRef<Impl>, std::string root);

        void readFile(const std::string& url, std::weak_ptr<Callback>);

    private:
        ActorRef<Impl> impl;
        const std::string root;
    };
    Actor<Worker> worker;
};

class AssetFileSource::Impl::Request : public AsyncRequest {
public:
    Request(std::shared_ptr<Callback> callback_) : callback(std::move(callback_)) {
    }

private:
    std::shared_ptr<Callback> callback;
};

AssetFileSource::Impl::Worker::Worker(ActorRef<Worker>, ActorRef<Impl> impl_, std::string root_)
    : impl(std::move(impl_)), root(std::move(root_)) {
}

void AssetFileSource::Impl::Worker::readFile(const std::string& url,
                                             std::weak_ptr<Callback> callback) {
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

    impl.invoke(&Impl::respond, callback, response);
}

AssetFileSource::Impl::Impl(Scheduler& scheduler, const std::string& root)
    : worker(scheduler, ActorRef<Impl>(*this, mailbox), root) {
}

std::unique_ptr<AssetFileSource::Impl::Request>
AssetFileSource::Impl::readFile(const Resource& resource, Callback callback) {
    auto cb = std::make_shared<Callback>(std::move(callback));
    worker.invoke(&Worker::readFile, resource.url, cb);
    return std::make_unique<Request>(std::move(cb));
}

void AssetFileSource::Impl::respond(std::weak_ptr<Callback> callback, Response response) {
    if (auto locked = callback.lock()) {
        (*locked)(response);
    }
}

AssetFileSource::AssetFileSource(Scheduler& scheduler, const std::string& root)
    : impl(std::make_unique<Impl>(scheduler, root)) {
}

AssetFileSource::~AssetFileSource() = default;

std::unique_ptr<AsyncRequest> AssetFileSource::request(const Resource& resource,
                                                       Callback callback) {
    return impl->readFile(resource, std::move(callback));
}

} // namespace mbgl
