#include <mbgl/mbgl.hpp>
#include <mbgl/android/jni.hpp>
#include <mbgl/storage/asset_request_baton.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/std.hpp>

#include <cerrno>
// NOTE a bug in the Android NDK breaks std::errno
// See https://code.google.com/p/android/issues/detail?id=72349
// After this is fixed change usage errno to std::errno
#include <zip.h>

namespace mbgl {

void AssetRequestBaton::run(AssetRequestBaton *ptr) {
    assert(std::this_thread::get_id() == ptr->threadId);

    if (ptr->canceled || !ptr->request) {
        // Either the AssetRequest object has been destructed, or the
        // request was canceled.
        cleanup(ptr);
        return;
    }

    int error = 0;
    struct zip *apk = zip_open(mbgl::android::apkPath.c_str(), 0, &error);
    if ((apk == nullptr) || ptr->canceled || !ptr->request) {
        // Opening the APK failed or was canceled. There isn't much left we can do.
        const int messageSize = zip_error_to_str(nullptr, 0, error, errno);
        const std::unique_ptr<char[]> message = mbgl::util::make_unique<char[]>(messageSize);
        zip_error_to_str(message.get(), 0, error, errno);
        notifyError(ptr, 500, message.get());
        cleanup(ptr);
        return;
    }

    std::string apkFilePath = "assets/" + ptr->path;
    struct zip_file *apkFile = zip_fopen(apk, apkFilePath.c_str(), ZIP_FL_NOCASE);
    if ((apkFile == nullptr) || ptr->canceled || !ptr->request) {
        // Opening the asset failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        zip_error_get(apk, &error, nullptr);
        notifyError(ptr, error == ZIP_ER_NOENT ? 404 : 500, zip_strerror(apk));
        zip_close(apk);
        apk = nullptr;
        cleanup(ptr);
        return;
    }

    struct zip_stat stat;
    if ((zip_stat(apk, apkFilePath.c_str(), ZIP_FL_NOCASE, &stat) != 0) || ptr->canceled || !ptr->request) {
        // Stating failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        notifyError(ptr, 500, zip_strerror(apk));
        zip_fclose(apkFile);
        apkFile = nullptr;
        zip_close(apk);
        apk = nullptr;
        cleanup(ptr);
        return;
    }

    const std::unique_ptr<char[]> data = mbgl::util::make_unique<char[]>(stat.size);

    if (static_cast<zip_uint64_t>(zip_fread(apkFile, reinterpret_cast<void *>(data.get()), stat.size)) != stat.size || ptr->canceled || !ptr->request) {
        // Reading failed or was canceled. We already have an open file handle
        // though, which we'll have to close.
        notifyError(ptr, 500, zip_file_strerror(apkFile));
        zip_fclose(apkFile);
        apkFile = nullptr;
        zip_close(apk);
        apk = nullptr;
        cleanup(ptr);
        return;
    }

    std::string body(data.get(), stat.size);
    notifySuccess(ptr, body);

    if (zip_fclose(apkFile) != 0) {
        // Closing the asset failed. But there isn't anything we can do.
    }
    apkFile = nullptr;

    if (zip_close(apk) != 0) {
        // Closing the APK failed. But there isn't anything we can do.
    }
    apk = nullptr;

    cleanup(ptr);
}

}
