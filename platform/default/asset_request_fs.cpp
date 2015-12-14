#include <mbgl/storage/asset_context_base.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/util.hpp>

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace {

struct FDHolder {
    FDHolder(int fd_) : fd(fd_) {}

    ~FDHolder() {
        if (fd >= 0) ::close(fd);
    }

    int fd;
};

class FileIOWorker {
public:
    FileIOWorker() = default;

    void open(const std::string& path, int flags, mode_t mode,
              std::function<void(std::unique_ptr<FDHolder>)> cb) {
        // FDHolder is needed because if the request gets canceled
        // we would otherwise end up with a fd leaking.
        int ret = ::open(path.c_str(), flags, mode);
        cb(std::make_unique<FDHolder>(ret < 0 ? -errno : ret));
    }

    void fstat(int fd, std::function<void (int, std::unique_ptr<struct stat>)> cb) {
        std::unique_ptr<struct stat> buf = std::make_unique<struct stat>();
        int ret = ::fstat(fd, buf.get());
        cb(ret < 0 ? -errno : ret, std::move(buf));
    }

    void read(int fd, size_t size,
              std::function<void (ssize_t, std::shared_ptr<std::string>)> cb) {
        std::shared_ptr<std::string> buf = std::make_shared<std::string>();
        buf->resize(size);

        ssize_t ret = ::read(fd, &buf->front(), size);
        cb(ret < 0 ? -errno : ret, std::move(buf));
    }

    void close(int fd, std::function<void (void)> cb) {
        ::close(fd);
        cb();
    }
};

} // namespace

namespace mbgl {

using namespace std::placeholders;

class AssetRequest : public RequestBase {
    MBGL_STORE_THREAD(tid)

public:
    AssetRequest(const Resource&, Callback, const std::string& assetRoot,
                 util::Thread<FileIOWorker> *worker);
    ~AssetRequest();

    // RequestBase implementation.
    void cancel() final;

private:
    void fileOpened(std::unique_ptr<FDHolder>);
    void fileStated(int result, std::unique_ptr<struct stat>);
    void fileRead(int result, std::shared_ptr<std::string>);
    void fileClosed();

    void notifyError(int result);

    void close();
    void cleanup();

    std::string assetRoot;
    std::unique_ptr<Response> response;

    int fd = -1;

    util::Thread<FileIOWorker> *worker;
    std::unique_ptr<WorkRequest> request;
};

class AssetFSContext : public AssetContextBase {
public:
    AssetFSContext()
        : worker({"FileIOWorker", util::ThreadType::Worker, util::ThreadPriority::Regular}) {}

private:
    RequestBase* createRequest(const Resource& resource,
                               RequestBase::Callback callback,
                               const std::string& assetRoot) final {
        return new AssetRequest(resource, callback, assetRoot, &worker);
    }

    util::Thread<FileIOWorker> worker;
};

AssetRequest::~AssetRequest() {
    MBGL_VERIFY_THREAD(tid);
}

AssetRequest::AssetRequest(const Resource& resource_, Callback callback_,
    const std::string& assetRoot_, util::Thread<FileIOWorker> *worker_)
    : RequestBase(resource_, callback_),
      assetRoot(assetRoot_),
      worker(worker_) {
    const auto &url = resource.url;
    std::string path;
    if (url.size() <= 8 || url[8] == '/') {
        // This is an empty or absolute path.
        path = mbgl::util::percentDecode(url.substr(8));
    } else {
        // This is a relative path. Prefix with the application root.
        path = assetRoot + "/" + mbgl::util::percentDecode(url.substr(8));
    }

    request = worker->invokeWithCallback(&FileIOWorker::open,
        std::bind(&AssetRequest::fileOpened, this, _1), path, O_RDONLY, S_IRUSR);
}

void AssetRequest::cancel() {
    MBGL_VERIFY_THREAD(tid);

    request.reset();
    close();
}

void AssetRequest::fileOpened(std::unique_ptr<FDHolder> holder) {
    MBGL_VERIFY_THREAD(tid);

    std::swap(fd, holder->fd);

    if (fd < 0) {
        // Opening failed. There isn't much left we can do.
        notifyError(fd);
        cleanup();

        return;
    }

    request = worker->invokeWithCallback(&FileIOWorker::fstat,
        std::bind(&AssetRequest::fileStated, this, _1, _2), fd);
}

void AssetRequest::fileStated(int result, std::unique_ptr<struct stat> stat) {
    MBGL_VERIFY_THREAD(tid);

    if (result != 0) {
        // Stating failed. We already have an open file handle
        // though, which we'll have to close.
        notifyError(result);
        close();
    } else {
        response = std::make_unique<Response>();
        response->modified = Seconds(stat->st_mtime);
        response->etag = util::toString(stat->st_ino);

        request = worker->invokeWithCallback(&FileIOWorker::read,
            std::bind(&AssetRequest::fileRead, this, _1, _2), fd, stat->st_size);
    }
}

void AssetRequest::fileRead(int result, std::shared_ptr<std::string> data) {
    MBGL_VERIFY_THREAD(tid);

    if (result < 0) {
        // Reading failed. We already have an open file handle
        // though, which we'll have to close.
        notifyError(result);
    } else {
        response->data = data;
        notify(std::move(response));
    }

    close();
}

void AssetRequest::fileClosed() {
    MBGL_VERIFY_THREAD(tid);

    cleanup();
}

void AssetRequest::close() {
    MBGL_VERIFY_THREAD(tid);

    if (fd >= 0) {
        request = worker->invokeWithCallback(&FileIOWorker::close,
            std::bind(&AssetRequest::fileClosed, this), fd);
        fd = -1;
    } else {
        cleanup();
    }
}

void AssetRequest::cleanup() {
    MBGL_VERIFY_THREAD(tid);

    delete this;
}

void AssetRequest::notifyError(int result) {
    MBGL_VERIFY_THREAD(tid);

    result = std::abs(result);
    Response::Error::Reason reason = Response::Error::Reason::Other;

    if (result == ENOENT || result == EISDIR) {
        reason = Response::Error::Reason::NotFound;
    }

    response = std::make_unique<Response>();
    response->error = std::make_unique<Response::Error>(reason, ::strerror(result));

    notify(std::move(response));
}

std::unique_ptr<AssetContextBase> AssetContextBase::createContext() {
    return std::make_unique<AssetFSContext>();
}

} // namespace mbgl
