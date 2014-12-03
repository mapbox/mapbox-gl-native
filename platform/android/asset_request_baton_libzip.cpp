#include <mbgl/android/jni.hpp>
#include <mbgl/storage/asset_request_baton.hpp>
#include <mbgl/storage/asset_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/std.hpp>

#include <cerrno>
// NOTE a bug in the Android NDK breaks std::errno
// See https://code.google.com/p/android/issues/detail?id=72349
// After this is fixed change usage errno to std::errno
#include <zip.h>

namespace mbgl {

void AssetRequestBaton::run(AssetRequestBaton *ptr) {
    assert(ptr->threadId == uv_thread_self());

    if (ptr->canceled || !ptr->request) {
        // Either the AssetRequest object has been destructed, or the
        // request was canceled.
        cleanup(ptr);
        return;
    }

    int error = 0;
    struct zip *apk = zip_open(mbgl::android::apk_path.c_str(), 0, &error);
    if ((apk == nullptr) || ptr->canceled || !ptr->request) {
        // Opening the APK failed or was canceled. There isn't much left we can do.
        const int message_size = zip_error_to_str(nullptr, 0, error, errno);
        const std::unique_ptr<char[]> message = mbgl::util::make_unique<char[]>(message_size);
        zip_error_to_str(message.get(), 0, error, errno);
        notify_error(ptr, 500, message.get());
        cleanup(ptr);
        return;
    }

    std::string apk_file_path = "assets/" + ptr->path;
    struct zip_file *apk_file = zip_fopen(apk, apk_file_path.c_str(), ZIP_FL_NOCASE);
    if ((apk_file == nullptr) || ptr->canceled || !ptr->request) {
        // Opening the asset failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        zip_error_get(apk, &error, nullptr);
        notify_error(ptr, error == ZIP_ER_NOENT ? 404 : 500, zip_strerror(apk));
        zip_close(apk);
        apk = nullptr;
        cleanup(ptr);
        return;
    }

    struct zip_stat stat;
    if ((zip_stat(apk, apk_file_path.c_str(), ZIP_FL_NOCASE, &stat) != 0) || ptr->canceled || !ptr->request) {
        // Stating failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        notify_error(ptr, 500, zip_strerror(apk));
        zip_fclose(apk_file);
        apk_file = nullptr;
        zip_close(apk);
        apk = nullptr;
        cleanup(ptr);
        return;
    }

    const std::unique_ptr<char[]> data = mbgl::util::make_unique<char[]>(stat.size);

    if (static_cast<zip_uint64_t>(zip_fread(apk_file, reinterpret_cast<void *>(data.get()), stat.size)) != stat.size || ptr->canceled || !ptr->request) {
        // Reading failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        notify_error(ptr, 500, zip_file_strerror(apk_file));
        zip_fclose(apk_file);
        apk_file = nullptr;
        zip_close(apk);
        apk = nullptr;
        cleanup(ptr);
        return;
    }

    if (ptr->request) {
        ptr->request->response = std::unique_ptr<Response>(new Response);
        ptr->request->response->code = 200;
        std::string body(data.get(), stat.size);
        ptr->request->response->data = body;
        ptr->request->notify();
    }

    if (zip_fclose(apk_file) != 0) {
        // Closing the asset failed. But there isn't anything we can do.
    }
    apk_file = nullptr;

    if (zip_close(apk) != 0) {
        // Closing the APK failed. But there isn't anything we can do.
    }
    apk = nullptr;

    cleanup(ptr);
}

}
