#include <mbgl/android/jni.hpp>
#include <mbgl/storage/asset_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/std.hpp>

#include <cerrno>
// NOTE a bug in the Android NDK breaks std::errno
// See https://code.google.com/p/android/issues/detail?id=72349
// After this is fixed change usage errno to std::errno
#include <zip.h>

namespace mbgl {

struct AssetRequestBaton {
    AssetRequestBaton(AssetRequest *request_, const std::string &path, uv_loop_t *loop);
    ~AssetRequestBaton();

    const std::thread::id threadId;
    AssetRequest *request = nullptr;
    std::unique_ptr<uv::async> asyncRun;
    std::string path;
    bool canceled = false;

    void cancel();
    static void notifyError(AssetRequestBaton *ptr, const int code, const char *message);
    static void notifySuccess(AssetRequestBaton *ptr, const std::string body);
    static void cleanup(AssetRequestBaton *ptr);
    static void run(AssetRequestBaton *ptr);
};

AssetRequestBaton::AssetRequestBaton(AssetRequest *request_, const std::string &path_, uv_loop_t *loop)
: threadId(std::this_thread::get_id()),
request(request_),
path(path_) {

    asyncRun = mbgl::util::make_unique<uv::async>(loop,  [this]() {
        run(this);
    });

    asyncRun->send();
}

AssetRequestBaton::~AssetRequestBaton() {
}

void AssetRequestBaton::cancel() {
    canceled = true;
}

void AssetRequestBaton::notifyError(AssetRequestBaton *ptr, const int code, const char *message) {
    assert(std::this_thread::get_id() == ptr->threadId);

    if (ptr->request && !ptr->canceled) {
        ptr->request->response = std::unique_ptr<Response>(new Response);
        ptr->request->response->code = code;
        ptr->request->response->message = message;
        ptr->request->notify();
    }
}

void AssetRequestBaton::notifySuccess(AssetRequestBaton *ptr, const std::string body) {
    assert(std::this_thread::get_id() == ptr->threadId);

    if (ptr->request && !ptr->canceled) {
        ptr->request->response = std::unique_ptr<Response>(new Response);
        ptr->request->response->code = 200;
        ptr->request->response->data = body;
        ptr->request->notify();
    }
}

void AssetRequestBaton::cleanup(AssetRequestBaton *ptr) {
    assert(std::this_thread::get_id() == ptr->threadId);

    if (ptr->request) {
        ptr->request->ptr = nullptr;
    }

    ptr->asyncRun.reset();

    delete ptr;
    ptr = nullptr;
}

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

AssetRequest::AssetRequest(const std::string &path_, uv_loop_t *loop)
: BaseRequest(path_) {
    if (!path.empty() && path[0] == '/') {
        // This is an absolute path. We don't allow this. Note that this is not a way to absolutely
        // prevent access to resources outside the application bundle; e.g. there could be symlinks
        // in the application bundle that link to outside. We don't care about these.
        response = util::make_unique<Response>();
        response->code = 403;
        response->message = "Path is outside the application bundle";
        notify();
    } else {
        // Note: The AssetRequestBaton object is deleted in AssetRequestBaton::cleanup().
        ptr = new AssetRequestBaton(this, path, loop);
    }
}

void AssetRequest::cancel() {
    assert(std::this_thread::get_id() == threadId);

    if (ptr) {
        ptr->cancel();

        // When deleting a AssetRequest object with a uv_fs_* call is in progress, we are making sure
        // that the callback doesn't accidentally reference this object again.
        ptr->request = nullptr;
        ptr = nullptr;
    }

    notify();
}

AssetRequest::~AssetRequest() {
    assert(std::this_thread::get_id() == threadId);
    cancel();

    // Note: The AssetRequestBaton object is deleted in AssetRequestBaton::cleanup().
}

}
