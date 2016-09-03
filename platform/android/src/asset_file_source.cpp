#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/url.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <zip.h>
#pragma GCC diagnostic pop

namespace {

struct ZipHolder {
    ZipHolder(struct zip* archive_) : archive(archive_) {}

    ~ZipHolder() {
        if (archive) ::zip_close(archive);
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

}

namespace mbgl {

class AssetFileRequest : public AsyncRequest {
public:
    AssetFileRequest(std::unique_ptr<WorkRequest> workRequest_)
        : workRequest(std::move(workRequest_)) {
    }

    std::unique_ptr<WorkRequest> workRequest;
};

class AssetFileSource::Impl {
public:
    Impl(const std::string& root_)
        : root(root_) {
    }

    void request(const std::string& url, FileSource::Callback callback) {
        ZipHolder archive = ::zip_open(root.c_str(), 0, nullptr);
        if (!archive.archive) {
            reportError(Response::Error::Reason::Other, "Could not open zip archive", callback);
            return;
        }

        struct zip_stat stat;
        ::zip_stat_init(&stat);

        std::string path = std::string("assets/") + mbgl::util::percentDecode(url.substr(8));

        int ret = ::zip_stat(archive.archive, path.c_str(), 0, &stat);
        if (ret < 0 || !(stat.valid & ZIP_STAT_SIZE)) {
            reportError(Response::Error::Reason::NotFound, "Could not stat file in zip archive", callback);
            return;
        }

        ZipFileHolder file = ::zip_fopen(archive.archive, path.c_str(), 0);
        if (!file.file) {
            reportError(Response::Error::Reason::NotFound, "Could not open file in zip archive", callback);
            return;
        }

        std::shared_ptr<std::string> buf = std::make_shared<std::string>();
        buf->resize(stat.size);

        ret = ::zip_fread(file.file, &buf->front(), stat.size);
        if (ret < 0) {
            reportError(Response::Error::Reason::Other, "Could not read file in zip archive", callback);
            return;
        }

        Response response;
        response.data = buf;
        callback(response);
    }

    void reportError(Response::Error::Reason reason, const char * message, FileSource::Callback callback) {
        Response response;
        response.error = std::make_unique<Response::Error>(reason, message);
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

}
