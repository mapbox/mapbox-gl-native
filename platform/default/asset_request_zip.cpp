#include <mbgl/storage/asset_context_base.hpp>
#include <mbgl/android/jni.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/uv.hpp>

#include <uv.h>
#include "uv_zip.h"

#include <map>
#include <cassert>
#include <forward_list>

namespace mbgl {

class AssetZipContext : public AssetContextBase {
public:
    explicit AssetZipContext(uv_loop_t *loop);
    ~AssetZipContext();

    RequestBase* createRequest(const Resource& resource,
                               RequestBase::Callback callback,
                               uv_loop_t* loop,
                               const std::string& assetRoot) final;

    uv_zip_t *getHandle(const std::string &path);
    void returnHandle(const std::string &path, uv_zip_t *zip);

    // A list of resuable uv_zip handles to avoid creating and destroying them all the time.
    std::map<std::string, std::forward_list<uv_zip_t *>> handles;
    uv_loop_t *loop;
};

AssetZipContext::AssetZipContext(uv_loop_t *loop_) : loop(loop_) {
}

uv_zip_t *AssetZipContext::getHandle(const std::string &path) {
    auto &list = handles[path];
    if (!list.empty()) {
        auto zip = list.front();
        list.pop_front();
        return zip;
    } else {
        return nullptr;
    }
}

void AssetZipContext::returnHandle(const std::string &path, uv_zip_t *zip) {
    handles[path].push_front(zip);
}

AssetZipContext::~AssetZipContext() {
    // Close all zip handles
    for (auto &list : handles) {
        for (auto zip : list.second) {
            uv_zip_discard(loop, zip, [](uv_zip_t *zip_) {
                uv_zip_cleanup(zip_);
                delete zip_;
            });
        }
    }
    handles.clear();
}

class AssetRequest : public RequestBase {
    MBGL_STORE_THREAD(tid)

public:
    AssetRequest(AssetZipContext&, const Resource&, Callback, const std::string& assetRoot);
    ~AssetRequest();

    void cancel() final;

private:
    AssetZipContext &context;
    bool cancelled = false;
    const std::string root;
    const std::string path;
    std::unique_ptr<Response> response;
    uv_buf_t buffer;

private:
    void openZipArchive();
    void archiveOpened(uv_zip_t *zip);
    void fileStated(uv_zip_t *zip);
    void fileOpened(uv_zip_t *zip);
    void fileRead(uv_zip_t *zip);
    void fileClosed(uv_zip_t *zip);
    void cleanup(uv_zip_t *zip);

    void notifyError(const char *message);
};

RequestBase* AssetZipContext::createRequest(const Resource& resource,
                                            RequestBase::Callback callback,
                                            uv_loop_t*,
                                            const std::string& assetRoot) {
    return new AssetRequest(*this, resource, callback, assetRoot);
}

AssetRequest::AssetRequest(AssetZipContext& context_, const Resource& resource_, Callback callback_, const std::string& assetRoot_)
    : RequestBase(resource_, callback_),
      context(context_),
      root(assetRoot_),
      path(std::string { "assets/" } + resource.url.substr(8)) {
    auto zip = context.getHandle(root);
    if (zip) {
        archiveOpened(zip);
    } else {
        openZipArchive();
    }
}

AssetRequest::~AssetRequest() {
    MBGL_VERIFY_THREAD(tid);
}

void AssetRequest::openZipArchive() {
    uv_fs_t *req = new uv_fs_t();
    req->data = this;

    // We're using uv_fs_open first to obtain a file descriptor. Then, uv_zip_fdopen will operate
    // on a read-only file.
    uv_fs_open(context.loop, req, root.c_str(), O_RDONLY, S_IRUSR, [](uv_fs_t *fsReq) {
        if (fsReq->result < 0) {
            auto impl = reinterpret_cast<AssetRequest *>(fsReq->data);
            impl->notifyError(uv::getFileRequestError(fsReq));
            delete impl;
        } else {
            uv_zip_t *zip = new uv_zip_t();
            uv_zip_init(zip);
            zip->data = fsReq->data;
            uv_zip_fdopen(fsReq->loop, zip, uv_file(fsReq->result), 0, [](uv_zip_t *openZip) {
                auto impl = reinterpret_cast<AssetRequest *>(openZip->data);
                if (openZip->result < 0) {
                    impl->notifyError(openZip->message);
                    delete openZip;
                    delete impl;
                } else {
                    impl->archiveOpened(openZip);
                }
            });
        }

        uv_fs_req_cleanup(fsReq);
        delete fsReq;
        fsReq = nullptr;
    });
}

#define INVOKE_MEMBER(name) \
    [](uv_zip_t *zip_) { \
        assert(zip_->data); \
        reinterpret_cast<AssetRequest *>(zip_->data)->name(zip_); \
    }

void AssetRequest::archiveOpened(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    zip->data = this;
    uv_zip_stat(context.loop, zip, path.c_str(), 0, INVOKE_MEMBER(fileStated));
}

void AssetRequest::fileStated(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    if (cancelled || zip->result < 0) {
        // Stat failed, probably because the file doesn't exist.
        if (zip->result < 0) {
            notifyError(zip->message);
        }
        cleanup(zip);
    } else if (!(zip->stat->valid & ZIP_STAT_SIZE)) {
        // We couldn't obtain the size of the file.
        notifyError("Could not determine file size in zip file");
        cleanup(zip);
    } else {
        response = std::make_unique<Response>();

        // Allocate the space for reading the data.
        response->data.resize(zip->stat->size);
        buffer = uv_buf_init(const_cast<char *>(response->data.data()), zip->stat->size);

        // Get the modification time in case we have one.
        if (zip->stat->valid & ZIP_STAT_MTIME) {
            response->modified = zip->stat->mtime;
        }

        if (zip->stat->valid & ZIP_STAT_INDEX) {
            response->etag = std::to_string(zip->stat->index);
        }

        uv_zip_fopen(context.loop, zip, path.c_str(), 0, INVOKE_MEMBER(fileOpened));
    }
}

void AssetRequest::fileOpened(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    if (zip->result < 0) {
        // Opening failed.
        notifyError(zip->message);
        cleanup(zip);
    } else if (cancelled) {
        // The request was canceled. Close the file again.
        uv_zip_fclose(context.loop, zip, zip->file, INVOKE_MEMBER(fileClosed));
    } else {
        uv_zip_fread(context.loop, zip, zip->file, &buffer, INVOKE_MEMBER(fileRead));
    }
}

void AssetRequest::fileRead(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    if (zip->result < 0) {
        // Reading failed. We still have an open file handle though.
        notifyError(zip->message);
    } else if (!cancelled) {
        response->status = Response::Successful;
        notify(std::move(response), FileCache::Hint::No);
    }

    uv_zip_fclose(context.loop, zip, zip->file, INVOKE_MEMBER(fileClosed));
}

void AssetRequest::fileClosed(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    if (zip->result < 0) {
        // Closing the file failed. But there isn't anything we can do.
    }

    cleanup(zip);
}

void AssetRequest::cleanup(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    context.returnHandle(root, zip);
    delete this;
}

void AssetRequest::notifyError(const char *message) {
    MBGL_VERIFY_THREAD(tid);

    if (!cancelled) {
        response = std::make_unique<Response>();
        response->status = Response::Error;
        response->message = message;
        notify(std::move(response), FileCache::Hint::No);
    } else {
        // The request was already canceled and deleted.
    }
}

void AssetRequest::cancel() {
    cancelled = true;
}

std::unique_ptr<AssetContextBase> AssetContextBase::createContext(uv_loop_t* loop) {
    return std::make_unique<AssetZipContext>(loop);
}

}
