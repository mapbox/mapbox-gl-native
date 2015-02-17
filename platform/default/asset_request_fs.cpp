#include <mbgl/storage/default/asset_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/uv.hpp>

#include <uv.h>

#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop

#include <cassert>


namespace algo = boost::algorithm;

namespace mbgl {

class AssetRequestImpl {
    MBGL_STORE_THREAD(tid)

public:
    AssetRequestImpl(AssetRequest *request, uv_loop_t *loop);
    ~AssetRequestImpl();

    static void fileOpened(uv_fs_t *req);
    static void fileStated(uv_fs_t *req);
    static void fileRead(uv_fs_t *req);
    static void fileClosed(uv_fs_t *req);
    static void notifyError(uv_fs_t *req);
    static void cleanup(uv_fs_t *req);


    AssetRequest *request = nullptr;
    bool canceled = false;
    uv_fs_t req;
    uv_file fd = -1;
    uv_buf_t buffer;
    std::unique_ptr<Response> response;
};

AssetRequestImpl::~AssetRequestImpl() {
    MBGL_VERIFY_THREAD(tid);

    if (request) {
        request->ptr = nullptr;
    }
}

AssetRequestImpl::AssetRequestImpl(AssetRequest *request_, uv_loop_t *loop) : request(request_) {
    req.data = this;

    const auto &url = request->resource.url;
    std::string path;
    if (url.size() <= 8 || url[8] == '/') {
        // This is an empty or absolute path.
        path = url.substr(8);
    } else {
        // This is a relative path. Prefix with the application root.
        path = request->source->assetRoot + "/" + url.substr(8);
    }

    uv_fs_open(loop, &req, path.c_str(), O_RDONLY, S_IRUSR, fileOpened);
}

void AssetRequestImpl::fileOpened(uv_fs_t *req) {
    assert(req->data);
    auto self = reinterpret_cast<AssetRequestImpl *>(req->data);
    MBGL_VERIFY_THREAD(self->tid);

    if (req->result < 0) {
        // Opening failed or was canceled. There isn't much left we can do.
        notifyError(req);
        cleanup(req);
    } else {
        const uv_file fd = uv_file(req->result);

        // We're going to reuse this handle, so we need to cleanup first.
        uv_fs_req_cleanup(req);

        if (self->canceled) {
            // The request was canceled.
            uv_fs_close(req->loop, req, fd, fileClosed);
        } else {
            self->fd = fd;
            uv_fs_fstat(req->loop, req, fd, fileStated);
        }
    }
}

void AssetRequestImpl::fileStated(uv_fs_t *req) {
    assert(req->data);
    auto self = reinterpret_cast<AssetRequestImpl *>(req->data);
    MBGL_VERIFY_THREAD(self->tid);

    if (req->result != 0 || self->canceled) {
        // Stating failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        notifyError(req);

        uv_fs_req_cleanup(req);
        uv_fs_close(req->loop, req, self->fd, fileClosed);
    } else {
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
        auto stat = static_cast<const uv_statbuf_t *>(req->ptr);
#else
        auto stat = static_cast<const uv_stat_t *>(req->ptr);
#endif
        if (stat->st_size > std::numeric_limits<int>::max()) {
            // File is too large for us to open this way because uv_buf's only support unsigned
            // ints as maximum size.
            auto response = util::make_unique<Response>();
            response->status = Response::Error;
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
            response->message = uv_strerror(uv_err_t {UV_EFBIG, 0});
#else
            response->message = uv_strerror(UV_EFBIG);
#endif
            assert(self->request);
            self->request->notify(std::move(response), FileCache::Hint::No);
            delete self->request;

            uv_fs_req_cleanup(req);
            uv_fs_close(req->loop, req, self->fd, fileClosed);
        } else {
            self->response = util::make_unique<Response>();
#ifdef __APPLE__
            self->response->modified = stat->st_mtimespec.tv_sec;
#else
            self->response->modified = stat->st_mtime;
#endif
            self->response->etag = std::to_string(stat->st_ino);
            const auto size = (unsigned int)(stat->st_size);
            self->response->data.resize(size);
            self->buffer = uv_buf_init(const_cast<char *>(self->response->data.data()), size);
            uv_fs_req_cleanup(req);
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
            uv_fs_read(req->loop, req, self->fd, self->buffer.base, self->buffer.len, -1, fileRead);
#else
            uv_fs_read(req->loop, req, self->fd, &self->buffer, 1, 0, fileRead);
#endif
        }
    }
}

void AssetRequestImpl::fileRead(uv_fs_t *req) {
    assert(req->data);
    auto self = reinterpret_cast<AssetRequestImpl *>(req->data);
    MBGL_VERIFY_THREAD(self->tid);

    if (req->result < 0 || self->canceled) {
        // Stating failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        notifyError(req);
    } else {
        // File was successfully read.
        self->response->status = Response::Successful;
        assert(self->request);
        self->request->notify(std::move(self->response), FileCache::Hint::No);
        delete self->request;
    }

    uv_fs_req_cleanup(req);
    uv_fs_close(req->loop, req, self->fd, fileClosed);
}

void AssetRequestImpl::fileClosed(uv_fs_t *req) {
    assert(req->data);
    auto self = reinterpret_cast<AssetRequestImpl *>(req->data);
    MBGL_VERIFY_THREAD(self->tid);
    (void(self)); // Silence unused variable error in Release mode

    if (req->result < 0) {
        // Closing the file failed. But there isn't anything we can do.
    }

    cleanup(req);
}

void AssetRequestImpl::notifyError(uv_fs_t *req) {
    assert(req->data);
    auto self = reinterpret_cast<AssetRequestImpl *>(req->data);
    MBGL_VERIFY_THREAD(self->tid);

    if (req->result < 0 && !self->canceled && req->result != UV_ECANCELED) {
        auto response = util::make_unique<Response>();
        response->status = Response::Error;
        response->message = uv::getFileRequestError(req);
        assert(self->request);
        self->request->notify(std::move(response), FileCache::Hint::No);
        delete self->request;
    }
}

void AssetRequestImpl::cleanup(uv_fs_t *req) {
    assert(req->data);
    auto self = reinterpret_cast<AssetRequestImpl *>(req->data);
    MBGL_VERIFY_THREAD(self->tid);
    uv_fs_req_cleanup(req);
    delete self;
}

// -------------------------------------------------------------------------------------------------

AssetRequest::AssetRequest(DefaultFileSource *source_, const Resource &resource_)
    : SharedRequestBase(source_, resource_) {
    assert(algo::starts_with(resource.url, "asset://"));
}

AssetRequest::~AssetRequest() {
    MBGL_VERIFY_THREAD(tid);

    if (ptr) {
        reinterpret_cast<AssetRequestImpl *>(ptr)->request = nullptr;
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
        reinterpret_cast<AssetRequestImpl *>(ptr)->canceled = true;

        // uv_cancel fails frequently when the request has already been started.
        // In that case, we have to let it complete and check the canceled bool
        // instead. The cancelation callback will delete the AssetRequest object.
        uv_cancel((uv_req_t *)&reinterpret_cast<AssetRequestImpl *>(ptr)->req);
    } else {
        // This request is canceled before we called start. We're safe to delete
        // ourselves now.
        delete this;
    }
}

}
