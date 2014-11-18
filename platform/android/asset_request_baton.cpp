#include <mbgl/android/jni.hpp>
#include <mbgl/platform/android/asset_request_baton.hpp>
#include <mbgl/platform/android/asset_request.hpp>
#include <mbgl/storage/response.hpp>

#include <limits>
#include <boost/make_unique.hpp>

namespace mbgl {

AssetRequestBaton::AssetRequestBaton(AssetRequest *request_, const std::string &path_, uv_loop_t *loop)
    : thread_id(uv_thread_self()),
      request(request_),
      async_run(new uv_async_t()),
      path(path_) {

    uv_async_init(loop, async_run.get(), run);
    async_run->data = this;

    uv_async_send(async_run.get());
}

AssetRequestBaton::~AssetRequestBaton() {
    uv_close((uv_handle_t *)async_run.get(), nullptr);
}

void AssetRequestBaton::cancel() {
    canceled = true;
}

void AssetRequestBaton::notify_error(uv_async_t *async, const int code, const char *message) {
    AssetRequestBaton *ptr = (AssetRequestBaton *)async->data;
    assert(ptr->thread_id == uv_thread_self());

    if (ptr->request && !ptr->canceled) {
        ptr->request->response = std::unique_ptr<Response>(new Response);
        ptr->request->response->code = code;
        ptr->request->response->message = message;
        ptr->request->notify();
    }
}

void AssetRequestBaton::run(uv_async_t *async) {
    AssetRequestBaton *ptr = (AssetRequestBaton *)async->data;
    assert(ptr->thread_id == uv_thread_self());

    if (ptr->canceled || !ptr->request) {
        // Either the AssetRequest object has been destructed, or the
        // request was canceled.
        cleanup(async);
        return;
    }

    int error = 0;
    ptr->apk = zip_open(mbgl::android::apk_path.c_str(), 0, &error);
    if ((ptr->apk == nullptr) || ptr->canceled || !ptr->request) {
        // Opening the APK failed or was canceled. There isn't much left we can do.
        const int message_size = zip_error_to_str(nullptr, 0, error, errno);
        const std::unique_ptr<char[]> message = boost::make_unique<char[]>(message_size);
        zip_error_to_str(message.get(), 0, error, errno);
        notify_error(async, 500, message.get());
        cleanup(async);
        return;
    }

    std::string apk_file_path = "assets/" + ptr->path;
    ptr->apk_file = zip_fopen(ptr->apk, apk_file_path.c_str(), ZIP_FL_NOCASE);
    if ((ptr->apk_file == nullptr) || ptr->canceled || !ptr->request) {
        // Opening the asset failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        zip_error_get(ptr->apk, &error, nullptr);
        notify_error(async, error == ZIP_ER_NOENT ? 404 : 500, zip_strerror(ptr->apk));
        zip_close(ptr->apk);
        ptr->apk = nullptr;
        cleanup(async);
        return;
    }

    struct zip_stat stat;
    if ((zip_stat(ptr->apk, apk_file_path.c_str(), ZIP_FL_NOCASE, &stat) != 0) ||
            ptr->canceled || !ptr->request) {
        // Stating failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        notify_error(async, 500, zip_strerror(ptr->apk));
        zip_fclose(ptr->apk_file);
        ptr->apk_file = nullptr;
        zip_close(ptr->apk);
        ptr->apk = nullptr;
        cleanup(async);
        return;
    }

    const std::unique_ptr<char[]> data = boost::make_unique<char[]>(stat.size);

    if (static_cast<zip_uint64_t>(zip_fread(ptr->apk_file, reinterpret_cast<void *>(data.get()), stat.size)) != stat.size ||
            ptr->canceled || !ptr->request) {
        // Reading failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        notify_error(async, 500, zip_file_strerror(ptr->apk_file));
        zip_fclose(ptr->apk_file);
        ptr->apk_file = nullptr;
        zip_close(ptr->apk);
        ptr->apk = nullptr;
        cleanup(async);
        return;
    }

    if (ptr->request) {
        ptr->request->response = std::unique_ptr<Response>(new Response);
        ptr->request->response->code = 200;
        std::string body(data.get(), stat.size);
        ptr->request->response->data = body;
        ptr->request->notify();
    }

    if (zip_fclose(ptr->apk_file) != 0) {
        // Closing the asset failed. But there isn't anything we can do.
    }
    ptr->apk_file = nullptr;

    if (zip_close(ptr->apk) != 0) {
        // Closing the APK failed. But there isn't anything we can do.
    }
    ptr->apk = nullptr;

    cleanup(async);
}

void AssetRequestBaton::cleanup(uv_async_t *async) {
    AssetRequestBaton *ptr = (AssetRequestBaton *)async->data;
    assert(ptr->thread_id == uv_thread_self());

    if (ptr->request) {
        ptr->request->ptr = nullptr;
    }

    delete ptr;
}

}
