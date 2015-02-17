#include <mbgl/storage/default/asset_request.hpp>
#include <mbgl/storage/default/thread_context.hpp>
#include <mbgl/android/jni.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/std.hpp>

#include "uv_zip.h"

#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop

#include <forward_list>

namespace algo = boost::algorithm;

namespace mbgl {

class AssetZipContext : public ThreadContext<AssetZipContext> {
public:
    AssetZipContext(uv_loop_t *loop);
    ~AssetZipContext();

    uv_zip_t *getHandle(const std::string &path);
    void returnHandle(const std::string &path, uv_zip_t *zip);

private:
    // A list of resuable uv_zip handles to avoid creating and destroying them all the time.
    std::map<std::string, std::forward_list<uv_zip_t *>> handles;
};

// -------------------------------------------------------------------------------------------------

template<> pthread_key_t ThreadContext<AssetZipContext>::key{};
template<> pthread_once_t ThreadContext<AssetZipContext>::once = PTHREAD_ONCE_INIT;

AssetZipContext::AssetZipContext(uv_loop_t *loop_) : ThreadContext(loop_) {
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

// -------------------------------------------------------------------------------------------------

class AssetRequestImpl {
    MBGL_STORE_THREAD(tid)

public:
    AssetRequestImpl(AssetRequest *request, uv_loop_t *loop);
    ~AssetRequestImpl();

    void cancel();

private:
    AssetZipContext &context;
    AssetRequest *request = nullptr;
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

// -------------------------------------------------------------------------------------------------

AssetRequestImpl::~AssetRequestImpl() {
    MBGL_VERIFY_THREAD(tid);

    if (request) {
        request->ptr = nullptr;
    }
}

AssetRequestImpl::AssetRequestImpl(AssetRequest *request_, uv_loop_t *loop)
    : context(*AssetZipContext::Get(loop)),
      request(request_),
      root(request->source->assetRoot),
      path(std::string { "assets/" } + request->resource.url.substr(8)) {
    auto zip = context.getHandle(root);
    if (zip) {
        archiveOpened(zip);
    } else {
        openZipArchive();
    }
}

void AssetRequestImpl::openZipArchive() {
    uv_fs_t *req = new uv_fs_t();
    req->data = this;

    assert(request);
    assert(request->source);

    // We're using uv_fs_open first to obtain a file descriptor. Then, uv_zip_fdopen will operate
    // on a read-only file.
    uv_fs_open(context.loop, req, root.c_str(), O_RDONLY, S_IRUSR, [](uv_fs_t *fsReq) {
        if (fsReq->result < 0) {
            auto impl = reinterpret_cast<AssetRequestImpl *>(fsReq->data);
            impl->notifyError(uv::getFileRequestError(fsReq));
            delete impl;
        } else {
            uv_zip_t *zip = new uv_zip_t();
            uv_zip_init(zip);
            zip->data = fsReq->data;
            uv_zip_fdopen(fsReq->loop, zip, uv_file(fsReq->result), 0, [](uv_zip_t *openZip) {
                auto impl = reinterpret_cast<AssetRequestImpl *>(openZip->data);
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
        reinterpret_cast<AssetRequestImpl *>(zip_->data)->name(zip_); \
    }

void AssetRequestImpl::archiveOpened(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    zip->data = this;
    uv_zip_stat(context.loop, zip, path.c_str(), 0, INVOKE_MEMBER(fileStated));
}

void AssetRequestImpl::fileStated(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    if (!request || zip->result < 0) {
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
        response = util::make_unique<Response>();

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

void AssetRequestImpl::fileOpened(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    if (zip->result < 0) {
        // Opening failed.
        notifyError(zip->message);
        cleanup(zip);
    } else if (!request) {
        // The request was canceled. Close the file again.
        uv_zip_fclose(context.loop, zip, zip->file, INVOKE_MEMBER(fileClosed));
    } else {
        uv_zip_fread(context.loop, zip, zip->file, &buffer, INVOKE_MEMBER(fileRead));
    }
}

void AssetRequestImpl::fileRead(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    if (zip->result < 0) {
        // Reading failed. We still have an open file handle though.
        notifyError(zip->message);
    } else if (request) {
        response->status = Response::Successful;
        request->notify(std::move(response), FileCache::Hint::No);
        delete request;
        assert(request == nullptr);
    }

    uv_zip_fclose(context.loop, zip, zip->file, INVOKE_MEMBER(fileClosed));
}

void AssetRequestImpl::fileClosed(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    if (zip->result < 0) {
        // Closing the file failed. But there isn't anything we can do.
    }

    cleanup(zip);
}

void AssetRequestImpl::cleanup(uv_zip_t *zip) {
    MBGL_VERIFY_THREAD(tid);

    context.returnHandle(root, zip);
    delete this;
}

void AssetRequestImpl::notifyError(const char *message) {
    MBGL_VERIFY_THREAD(tid);

    if (request) {
        response = util::make_unique<Response>();
        response->status = Response::Error;
        response->message = message;
        request->notify(std::move(response), FileCache::Hint::No);
        delete request;
        assert(request == nullptr);
    } else {
        // The request was already canceled and deleted.
    }
}

void AssetRequestImpl::cancel() {
    request = nullptr;
}

// -------------------------------------------------------------------------------------------------

AssetRequest::AssetRequest(DefaultFileSource *source_, const Resource &resource_)
    : SharedRequestBase(source_, resource_) {
    assert(algo::starts_with(resource.url, "asset://"));
}

AssetRequest::~AssetRequest() {
    MBGL_VERIFY_THREAD(tid);

    if (ptr) {
        reinterpret_cast<AssetRequestImpl *>(ptr)->cancel();
    }
}

void AssetRequest::start(uv_loop_t *loop, std::unique_ptr<Response> response) {
    MBGL_VERIFY_THREAD(tid);

    // We're ignoring the existing response if any.
    (void(response));

    assert(!ptr);
    ptr = new AssetRequestImpl(this, loop);
    // Note: the AssetRequestImpl deletes itself.
}

void AssetRequest::cancel() {
    MBGL_VERIFY_THREAD(tid);

    if (ptr) {
        reinterpret_cast<AssetRequestImpl *>(ptr)->cancel();
    }

    delete this;
}

}
