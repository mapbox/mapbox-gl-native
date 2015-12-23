#include <mbgl/storage/asset_context_base.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/thread.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <zip.h>
#pragma GCC diagnostic pop

#include <cassert>
#include <forward_list>
#include <map>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace {

struct ZipHolder {
    ZipHolder(struct zip* archive_) : archive(archive_) {}

    ~ZipHolder() {
        if (archive) ::zip_discard(archive);
    }

    struct zip* archive;
};

struct ZipFileHolder {
    ZipFileHolder(struct zip_file* file_) : file(file_) {}

    ~ZipFileHolder() {
        if (file) ::zip_fclose(file);
    }

    struct zip_file* file;
};

class ZipIOWorker {
public:
    ZipIOWorker() = default;

    void zip_fdopen(const std::string& path,
                    std::function<void(std::unique_ptr<ZipHolder>)> cb) {
        int fd = ::open(path.c_str(), O_RDONLY, S_IRUSR);
        if (fd < 0) {
            cb(std::make_unique<ZipHolder>(nullptr));
        } else {
            int errorp;
            struct zip* archive = ::zip_fdopen(fd, 0, &errorp);
            cb(std::make_unique<ZipHolder>(archive));
        }
    }

    void zip_stat(struct zip* archive, const std::string& path,
                  std::function<void (int, std::unique_ptr<struct zip_stat>)> cb) {
        std::unique_ptr<struct zip_stat> buf = std::make_unique<struct zip_stat>();
        ::zip_stat_init(buf.get());

        int ret = ::zip_stat(archive, path.c_str(), 0,  buf.get());
        cb(ret, std::move(buf));
    }

    void zip_fopen(struct zip* archive, const std::string& path,
                   std::function<void (std::unique_ptr<ZipFileHolder>)> cb) {
        struct zip_file* file = ::zip_fopen(archive, path.c_str(), 0);
        cb(std::make_unique<ZipFileHolder>(file));
    }

    void zip_fread(struct zip_file* file, size_t size,
                   std::function<void (int, std::shared_ptr<std::string>)> cb) {
        std::shared_ptr<std::string> buf = std::make_shared<std::string>();
        buf->resize(size);

        int ret = ::zip_fread(file, &buf->front(), size);
        cb(ret, std::move(buf));
    }

    void zip_fclose(struct zip_file* file, std::function<void (int)> cb) {
        cb(::zip_fclose(file));
    }
};

}

namespace mbgl {

using namespace std::placeholders;

class AssetZipContext;

class AssetRequest : public RequestBase {
    MBGL_STORE_THREAD(tid)

public:
    AssetRequest(const std::string& url, Callback, const std::string& assetRoot,
                 AssetZipContext* context, util::Thread<ZipIOWorker>* worker);
    ~AssetRequest();

    // RequestBase implementation.
    void cancel() final;

private:
    void openArchive();
    void archiveOpened(std::unique_ptr<ZipHolder>);
    void fileStated(int result, std::unique_ptr<struct zip_stat>);
    void fileOpened(std::unique_ptr<ZipFileHolder>);
    void fileRead(int result, std::shared_ptr<std::string>);
    void fileClosed();

    void close();
    void cleanup();

    void notifyError(Response::Error::Reason, const char *message);

    const std::string root;
    const std::string path;
    std::unique_ptr<Response> response;

    struct zip* archive = nullptr;
    struct zip_file* file = nullptr;
    size_t size = 0;

    AssetZipContext *context;

    util::Thread<ZipIOWorker> *worker;
    std::unique_ptr<WorkRequest> request;
};

class AssetZipContext : public AssetContextBase {
public:
    AssetZipContext();
    ~AssetZipContext();

    RequestBase* createRequest(const std::string& url,
                               RequestBase::Callback callback,
                               const std::string& assetRoot) final {
        return new AssetRequest(url, callback, assetRoot, this, &worker);
    }

    struct zip *getHandle(const std::string &path);
    void returnHandle(const std::string &path, struct zip *zip);

    // A list of resuable zip handles to avoid creating
    // and destroying them all the time.
    std::map<std::string, std::forward_list<struct zip*>> handles;

    util::Thread<ZipIOWorker> worker;
    std::unique_ptr<WorkRequest> request;
};

AssetZipContext::AssetZipContext()
        : worker({"ZipIOWorker", util::ThreadType::Worker, util::ThreadPriority::Regular}) {}

AssetZipContext::~AssetZipContext() {
    // Close all zip handles
    for (auto &list : handles) {
        for (auto archive : list.second) {
            zip_discard(archive);
        }
    }

    handles.clear();
}

struct zip* AssetZipContext::getHandle(const std::string &path) {
    auto &list = handles[path];
    if (!list.empty()) {
        auto archive = list.front();
        list.pop_front();
        return archive;
    } else {
        return nullptr;
    }
}

void AssetZipContext::returnHandle(const std::string &path, struct zip* archive) {
    handles[path].push_front(archive);
}

AssetRequest::AssetRequest(const std::string& url_, Callback callback_,
    const std::string& assetRoot, AssetZipContext* context_, util::Thread<ZipIOWorker>* worker_)
    : RequestBase(url_, callback_),
      root(assetRoot),
      path(std::string("assets/") + url.substr(8)),
      context(context_),
      worker(worker_) {
    archive = context->getHandle(root);
    if (archive) {
        request = worker->invokeWithCallback(&ZipIOWorker::zip_stat,
            std::bind(&AssetRequest::fileStated, this, _1, _2), archive, path);
    } else {
        request = worker->invokeWithCallback(&ZipIOWorker::zip_fdopen,
            std::bind(&AssetRequest::archiveOpened, this, _1), root);
    }
}

AssetRequest::~AssetRequest() {
    MBGL_VERIFY_THREAD(tid);
}

void AssetRequest::cancel() {
    MBGL_VERIFY_THREAD(tid);

    request.reset();
    close();
}

void AssetRequest::archiveOpened(std::unique_ptr<ZipHolder> holder) {
    MBGL_VERIFY_THREAD(tid);

    std::swap(archive, holder->archive);

    if (!archive) {
        notifyError(Response::Error::Reason::Other, "Could not open zip archive");
        cleanup();
    } else {
        request = worker->invokeWithCallback(&ZipIOWorker::zip_stat,
            std::bind(&AssetRequest::fileStated, this, _1, _2), archive, path);
    }
}

void AssetRequest::fileStated(int result, std::unique_ptr<struct zip_stat> stat) {
    MBGL_VERIFY_THREAD(tid);

    if (result < 0 || !(stat->valid & ZIP_STAT_SIZE)) {
        notifyError(Response::Error::Reason::NotFound, "Could not stat file in zip archive");
        cleanup();
    } else {
        response = std::make_unique<Response>();

        if (stat->valid & ZIP_STAT_MTIME) {
            response->modified = Seconds(stat->mtime);
        }

        if (stat->valid & ZIP_STAT_INDEX) {
            response->etag = std::to_string(stat->index);
        }

        size = stat->size;

        request = worker->invokeWithCallback(&ZipIOWorker::zip_fopen,
            std::bind(&AssetRequest::fileOpened, this, _1), archive, path);
    }
}

void AssetRequest::fileOpened(std::unique_ptr<ZipFileHolder> holder) {
    MBGL_VERIFY_THREAD(tid);

    std::swap(file, holder->file);

    if (!file) {
        notifyError(Response::Error::Reason::NotFound, "Could not open file in zip archive"),
        cleanup();
    } else {
        request = worker->invokeWithCallback(&ZipIOWorker::zip_fread,
            std::bind(&AssetRequest::fileRead, this, _1, _2), file, size);
    }
}

void AssetRequest::fileRead(int result, std::shared_ptr<std::string> data) {
    MBGL_VERIFY_THREAD(tid);

    if (result < 0) {
        notifyError(Response::Error::Reason::Other, "Could not read file in zip archive");
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

    if (file) {
        request = worker->invokeWithCallback(&ZipIOWorker::zip_fclose,
            std::bind(&AssetRequest::fileClosed, this), file);
        file = nullptr;
    } else {
        cleanup();
    }
}

void AssetRequest::cleanup() {
    MBGL_VERIFY_THREAD(tid);

    if (archive) {
        context->returnHandle(root, archive);
    }

    delete this;
}

void AssetRequest::notifyError(Response::Error::Reason reason, const char *message) {
    MBGL_VERIFY_THREAD(tid);

    response = std::make_unique<Response>();
    response->error = std::make_unique<Response::Error>(reason, message);

    notify(std::move(response));
}

std::unique_ptr<AssetContextBase> AssetContextBase::createContext() {
    return std::make_unique<AssetZipContext>();
}

}
