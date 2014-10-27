#ifndef MBGL_STORAGE_FILE_REQUEST_BATON
#define MBGL_STORAGE_FILE_REQUEST_BATON

#include <mbgl/storage/file_request.hpp>

#include <uv.h>

namespace mbgl {

struct FileRequestBaton {
    FileRequestBaton(FileRequest *request_, const std::string &path, uv_loop_t *loop);
    ~FileRequestBaton();

    void cancel();
    static void file_opened(uv_fs_t *req);
    static void file_stated(uv_fs_t *req);
    static void file_read(uv_fs_t *req);
    static void file_closed(uv_fs_t *req);
    static void notify_error(uv_fs_t *req);
    static void cleanup(uv_fs_t *req);

    const unsigned long thread_id;
    FileRequest *request = nullptr;
    uv_fs_t req;
    uv_file fd = -1;
    bool canceled = false;
    std::string body;
    uv_buf_t buffer;
};


}


#endif
