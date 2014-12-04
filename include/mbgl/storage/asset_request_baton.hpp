#ifndef MBGL_STORAGE_ASSET_REQUEST_BATON
#define MBGL_STORAGE_ASSET_REQUEST_BATON

#include <mbgl/storage/asset_request.hpp>
#include <mbgl/util/uv.hpp>
#include <thread>

#include <uv.h>

namespace mbgl {

struct AssetRequestBaton {
    AssetRequestBaton(AssetRequest *request_, const std::string &path, uv_loop_t *loop);

    const std::thread::id threadId;
    AssetRequest *request = nullptr;
    std::unique_ptr<uv::async> asyncRun;
    std::string path;
    bool canceled = false;

    void cancel();
    static void notify_error(AssetRequestBaton *ptr, const int code, const char *message);
    static void cleanup(AssetRequestBaton *ptr);

    // IMPLEMENT THIS PLATFORM SPECIFIC FUNCTION:

    // Called to load the asset. Platform-specific implementation.
    static void run(AssetRequestBaton *ptr);

};


}


#endif
