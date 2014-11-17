#ifndef MBGL_STORAGE_ASSET_REQUEST_BATON
#define MBGL_STORAGE_ASSET_REQUEST_BATON

#include <mbgl/platform/android/asset_request.hpp>

#include <uv.h>
#include <zip.h>

namespace mbgl {

struct AssetRequestBaton {
    AssetRequestBaton(AssetRequest *request_, const std::string &path, uv_loop_t *loop);
    ~AssetRequestBaton();

    void cancel();
    static void run(uv_async_t *async);
    static void notify_error(uv_async_t *async, const int code, const char *message);
    static void cleanup(uv_async_t *async);

    const unsigned long thread_id;
    AssetRequest *request = nullptr;
    std::unique_ptr<uv_async_t> async_run;
    struct zip *apk = nullptr;
    struct zip_file *apk_file = nullptr;
    std::string path;
    bool canceled = false;
};


}


#endif
