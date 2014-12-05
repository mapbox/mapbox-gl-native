#ifndef MBGL_STORAGE_FILE_REQUEST_BATON
#define MBGL_STORAGE_FILE_REQUEST_BATON

#include <mbgl/storage/file_request.hpp>
#include <thread>

#include <uv.h>

namespace mbgl {

struct FileRequestBaton {
    FileRequestBaton(FileRequest *request_, const std::string &path, uv_loop_t *loop);
    ~FileRequestBaton();

    void cancel();
    static void fileOpened(uv_fs_t *req);
    static void fileStated(uv_fs_t *req);
    static void fileRead(uv_fs_t *req);
    static void fileClosed(uv_fs_t *req);
    static void notifyError(uv_fs_t *req);
    static void cleanup(uv_fs_t *req);

    const std::thread::id threadId;
    FileRequest *request = nullptr;
    uv_fs_t req;
    uv_file fd = -1;
    bool canceled = false;
    std::string body;
    uv_buf_t buffer;
};


}


#endif
