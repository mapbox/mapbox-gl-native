#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/actor/actor.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/io.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace mbgl {

class RequestCallback {
public:
    RequestCallback(ActorRef<RequestCallback>, FileSource::Callback callback_)
        :callback(callback_) {
    };

    ~RequestCallback() = default;

    void operator() (Response response) {
        callback(response);
    }

private:
    FileSource::Callback callback;
};

class AssetRequest {
public:
    AssetRequest(ActorRef<AssetRequest>) {

    }
    ~AssetRequest() = default;

    void send(const std::string root, const Resource& resource, ActorRef<RequestCallback> callback) {
        std::string path;

        auto url = resource.url;

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
                        Response::Error::Reason::Other,
                        util::toString(std::current_exception()));
            }
        }

        callback.invoke(&RequestCallback::operator(), response);
    }
};

class AssetRequestHandle : public AsyncRequest {
public:
    AssetRequestHandle(Scheduler& scheduler, const std::string root, const Resource& resource, FileSource::Callback callback_)
        : request(scheduler)
        , callback(scheduler, callback_) {
        request.invoke(&AssetRequest::send, root, resource, callback.self());
    }

    ~AssetRequestHandle() = default;

private:
    Actor<AssetRequest> request;
    Actor<RequestCallback> callback;
};


class AssetFileSource::Impl {
public:
    Impl(Scheduler& scheduler_, const std::string& root_)
            : scheduler(scheduler_)
            , root(root_) {
    }

    std::unique_ptr<AsyncRequest> request(const Resource& resource, Callback callback) {
        return std::make_unique<AssetRequestHandle>(scheduler, root, resource, callback);
    }

private:
    Scheduler& scheduler;
    const std::string root;
};

AssetFileSource::AssetFileSource(Scheduler& scheduler, const std::string& root)
    : impl(std::make_unique<Impl>(scheduler, root)) {
}

AssetFileSource::~AssetFileSource() = default;

std::unique_ptr<AsyncRequest> AssetFileSource::request(const Resource& resource, Callback callback) {
    return impl->request(resource, callback);
}

} // namespace mbgl
