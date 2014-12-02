#ifndef MBGL_STORAGE_ASSET_REQUEST_BATON
#define MBGL_STORAGE_ASSET_REQUEST_BATON

#include <mbgl/storage/asset_request.hpp>

#include <uv.h>

namespace mbgl {

struct AssetRequestBaton {
    AssetRequestBaton(AssetRequest *request_, const std::string &path, uv_loop_t *loop);
    ~AssetRequestBaton();

    const unsigned long thread_id;
    AssetRequest *request = nullptr;
    std::unique_ptr<uv_async_t> async_run;
    std::string path;
    bool canceled = false;

    void cancel();
    static void notify_error(uv_async_t *async, const int code, const char *message);
    static void cleanup(uv_async_t *async);

    // IMPLEMENT THIS PLATFORM SPECIFIC FUNCTION:

    // Called to load the asset. Platform-specific implementation.
    static void run(uv_async_t *async);

};


}


#endif
