#include <mbgl/storage/file_request_baton.hpp>
#include <mbgl/storage/file_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/std.hpp>

#include <limits>

namespace mbgl {

FileRequestBaton::FileRequestBaton(FileRequest *request_, const std::string &path, uv_loop_t *loop)
    : threadId(std::this_thread::get_id()), request(request_) {
    req.data = this;
    uv_fs_open(loop, &req, path.c_str(), O_RDONLY, S_IRUSR, file_opened);
}

FileRequestBaton::~FileRequestBaton() {
}

void FileRequestBaton::cancel() {
    canceled = true;

    // uv_cancel fails frequently when the request has already been started.
    // In that case, we have to let it complete and check the canceled bool
    // instead.
    uv_cancel((uv_req_t *)&req);
}

void FileRequestBaton::notify_error(uv_fs_t *req) {
    FileRequestBaton *ptr = reinterpret_cast<FileRequestBaton *>(req->data);
    assert(std::this_thread::get_id() == ptr->threadId);

    if (ptr->request && req->result < 0 && !ptr->canceled && req->result != UV_ECANCELED) {
        ptr->request->response = util::make_unique<Response>();
        ptr->request->response->code = req->result == UV_ENOENT ? 404 : 500;
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
        ptr->request->response->message = uv_strerror(uv_last_error(req->loop));
#else
        ptr->request->response->message = uv_strerror(int(req->result));
#endif
        ptr->request->notify();
    }
}

void FileRequestBaton::file_opened(uv_fs_t *req) {
    FileRequestBaton *ptr = reinterpret_cast<FileRequestBaton *>(req->data);
    assert(std::this_thread::get_id() == ptr->threadId);

    if (req->result < 0) {
        // Opening failed or was canceled. There isn't much left we can do.
        notify_error(req);
        cleanup(req);
    } else {
        const uv_file fd = uv_file(req->result);

        // We're going to reuse this handle, so we need to cleanup first.
        uv_fs_req_cleanup(req);

        if (ptr->canceled || !ptr->request) {
            // Either the FileRequest object has been destructed, or the
            // request was canceled.
            uv_fs_close(req->loop, req, fd, file_closed);
        } else {
            ptr->fd = fd;
            uv_fs_fstat(req->loop, req, fd, file_stated);
        }
    }
}

void FileRequestBaton::file_stated(uv_fs_t *req) {
    FileRequestBaton *ptr = reinterpret_cast<FileRequestBaton *>(req->data);
    assert(std::this_thread::get_id() == ptr->threadId);

    if (req->result != 0 || ptr->canceled || !ptr->request) {
        // Stating failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        notify_error(req);

        uv_fs_req_cleanup(req);
        uv_fs_close(req->loop, req, ptr->fd, file_closed);
    } else {
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
        const uv_statbuf_t *stat = static_cast<const uv_statbuf_t *>(req->ptr);
#else
        const uv_stat_t *stat = static_cast<const uv_stat_t *>(req->ptr);
#endif
        if (stat->st_size > std::numeric_limits<int>::max()) {
            // File is too large for us to open this way because uv_buf's only support unsigned
            // ints as maximum size.
            if (ptr->request) {
                ptr->request->response = util::make_unique<Response>();
                ptr->request->response->code = UV_EFBIG;
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
                ptr->request->response->message = uv_strerror(uv_err_t {UV_EFBIG, 0});
#else
                ptr->request->response->message = uv_strerror(UV_EFBIG);
#endif
                ptr->request->notify();
            }

            uv_fs_req_cleanup(req);
            uv_fs_close(req->loop, req, ptr->fd, file_closed);
        } else {
            const unsigned int size = (unsigned int)(stat->st_size);
            ptr->body.resize(size);
            ptr->buffer = uv_buf_init(const_cast<char *>(ptr->body.data()), size);
            uv_fs_req_cleanup(req);
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
            uv_fs_read(req->loop, req, ptr->fd, ptr->buffer.base, ptr->buffer.len, -1, file_read);
#else
            uv_fs_read(req->loop, req, ptr->fd, &ptr->buffer, 1, 0, file_read);
#endif
        }
    }
}

void FileRequestBaton::file_read(uv_fs_t *req) {
    FileRequestBaton *ptr = reinterpret_cast<FileRequestBaton *>(req->data);
    assert(std::this_thread::get_id() == ptr->threadId);

    if (req->result < 0 || ptr->canceled || !ptr->request) {
        // Reading failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        notify_error(req);
    } else {
        // File was successfully read.
        if (ptr->request) {
            ptr->request->response = util::make_unique<Response>();
            ptr->request->response->code = 200;
            ptr->request->response->data = std::move(ptr->body);
            ptr->request->notify();
        }
    }

    uv_fs_req_cleanup(req);
    uv_fs_close(req->loop, req, ptr->fd, file_closed);
}

void FileRequestBaton::file_closed(uv_fs_t *req) {
    assert(std::this_thread::get_id() == reinterpret_cast<FileRequestBaton *>(req->data)->thread_id);

    if (req->result < 0) {
        // Closing the file failed. But there isn't anything we can do.
    }

    cleanup(req);
}

void FileRequestBaton::cleanup(uv_fs_t *req) {
    FileRequestBaton *ptr = reinterpret_cast<FileRequestBaton *>(req->data);
    assert(std::this_thread::get_id() == ptr->threadId);

    if (ptr->request) {
        ptr->request->ptr = nullptr;
    }

    uv_fs_req_cleanup(req);
    delete ptr;
    ptr = nullptr;
}

}
