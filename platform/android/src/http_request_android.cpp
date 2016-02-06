#include <mbgl/storage/http_context_base.hpp>
#include <mbgl/storage/http_request_base.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/platform/log.hpp>
#include "jni.hpp"

#include <mbgl/util/async_task.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/string.hpp>

#include <jni.h>

namespace mbgl {

void JNICALL nativeOnFailure(JNIEnv *env, jobject obj, jlong nativePtr, jint type, jstring message);
void JNICALL nativeOnResponse(JNIEnv *env, jobject obj, jlong nativePtr, jint code, jstring message, jstring etag, jstring modified, jstring cacheControl, jstring expires, jbyteArray body);

class HTTPAndroidRequest;

class HTTPAndroidContext : public HTTPContextBase {
public:
    explicit HTTPAndroidContext();
    ~HTTPAndroidContext();

    HTTPRequestBase* createRequest(const Resource&, HTTPRequestBase::Callback) final;

    JavaVM *vm = nullptr;
    jobject obj = nullptr;
};

class HTTPAndroidRequest : public HTTPRequestBase {
public:
    HTTPAndroidRequest(HTTPAndroidContext*, const Resource&, Callback);
    ~HTTPAndroidRequest();

    void cancel() final;

    void onFailure(JNIEnv*, int type, jstring message);
    void onResponse(JNIEnv*, int code, jstring message, jstring etag, jstring modified, jstring cacheControl, jstring expires, jbyteArray body);

private:
    void finish();

    HTTPAndroidContext *context = nullptr;

    bool cancelled = false;

    std::unique_ptr<Response> response;
    const std::shared_ptr<const Response> existingResponse;

    jobject obj = nullptr;

    util::AsyncTask async;

    static const int connectionError = 0;
    static const int temporaryError = 1;
    static const int permanentError = 2;
    static const int canceledError = 3;
};

// -------------------------------------------------------------------------------------------------

HTTPAndroidContext::HTTPAndroidContext()
    : vm(mbgl::android::theJVM) {

    JNIEnv *env = nullptr;
    bool detach = mbgl::android::attach_jni_thread(vm, &env, "HTTPAndroidContext::HTTPAndroidContext()");

    const std::vector<JNINativeMethod> methods = {
        {"nativeOnFailure", "(JILjava/lang/String;)V", reinterpret_cast<void *>(&nativeOnFailure)},
        {"nativeOnResponse",
         "(JILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[B)V",
         reinterpret_cast<void *>(&nativeOnResponse)}
    };

    if (env->RegisterNatives(mbgl::android::httpRequestClass, methods.data(), methods.size()) < 0) {
        env->ExceptionDescribe();
    }

    obj = env->CallStaticObjectMethod(mbgl::android::httpContextClass, mbgl::android::httpContextGetInstanceId);
    if (env->ExceptionCheck() || (obj == nullptr)) {
        env->ExceptionDescribe();
    }

    obj = env->NewGlobalRef(obj);
    if (obj == nullptr) {
        env->ExceptionDescribe();
    }

    mbgl::android::detach_jni_thread(vm, &env, detach);
}

HTTPAndroidContext::~HTTPAndroidContext() {
    JNIEnv *env = nullptr;
    bool detach = mbgl::android::attach_jni_thread(vm, &env, "HTTPAndroidContext::~HTTPAndroidContext()");

    env->DeleteGlobalRef(obj);
    obj = nullptr;

    mbgl::android::detach_jni_thread(vm, &env, detach);

    vm = nullptr;
}

HTTPRequestBase* HTTPAndroidContext::createRequest(const Resource& resource, HTTPRequestBase::Callback callback) {
    return new HTTPAndroidRequest(this, resource, callback);
}

HTTPAndroidRequest::HTTPAndroidRequest(HTTPAndroidContext* context_, const Resource& resource_, Callback callback_)
    : HTTPRequestBase(resource_, callback_),
      context(context_),
      async([this] { finish(); }) {

    std::string etagStr;
    std::string modifiedStr;

    if (resource.priorEtag) {
        etagStr = *resource.priorEtag;
    } else if (resource.priorModified) {
        modifiedStr = util::rfc1123(*resource.priorModified);
    }

    JNIEnv *env = nullptr;
    bool detach = mbgl::android::attach_jni_thread(context->vm, &env, "HTTPAndroidContext::HTTPAndroidRequest()");

    jstring resourceUrl = mbgl::android::std_string_to_jstring(env, resource.url);
    jstring userAgent = mbgl::android::std_string_to_jstring(env, "MapboxGL/1.0");
    jstring etag = mbgl::android::std_string_to_jstring(env, etagStr);
    jstring modified = mbgl::android::std_string_to_jstring(env, modifiedStr);
    obj = env->CallObjectMethod(context->obj, mbgl::android::httpContextCreateRequestId, reinterpret_cast<jlong>(this), resourceUrl, userAgent, etag, modified);
    if (env->ExceptionCheck() || (obj == nullptr)) {
      env->ExceptionDescribe();
    }

    obj = env->NewGlobalRef(obj);
    if (obj == nullptr) {
      env->ExceptionDescribe();
    }

    env->CallVoidMethod(obj, mbgl::android::httpRequestStartId);
    if (env->ExceptionCheck()) {
      env->ExceptionDescribe();
    }

    mbgl::android::detach_jni_thread(context->vm, &env, detach);
}

HTTPAndroidRequest::~HTTPAndroidRequest() {
    JNIEnv *env = nullptr;
    bool detach = mbgl::android::attach_jni_thread(context->vm, &env, "HTTPAndroidContext::~HTTPAndroidRequest()");

    env->DeleteGlobalRef(obj);
    obj = nullptr;

    mbgl::android::detach_jni_thread(context->vm, &env, detach);
}

void HTTPAndroidRequest::cancel() {
    cancelled = true;

    JNIEnv *env = nullptr;
    bool detach = mbgl::android::attach_jni_thread(context->vm, &env, "HTTPAndroidContext::cancel()");

    env->CallVoidMethod(obj, mbgl::android::httpRequestCancelId);
    if (env->ExceptionCheck()) {
      env->ExceptionDescribe();
    }

    mbgl::android::detach_jni_thread(context->vm, &env, detach);
}

void HTTPAndroidRequest::finish() {
    if (!cancelled) {
        assert(response);
        notify(*response);
    }

    delete this;
}

void HTTPAndroidRequest::onResponse(JNIEnv* env, int code, jstring /* message */, jstring etag, jstring modified, jstring cacheControl, jstring expires, jbyteArray body) {
    response = std::make_unique<Response>();
    using Error = Response::Error;

    if (etag != nullptr) {
        response->etag = mbgl::android::std_string_from_jstring(env, etag);
    }

    if (modified != nullptr) {
        response->modified = util::parseTimePoint(mbgl::android::std_string_from_jstring(env, modified).c_str());
    }

    if (cacheControl != nullptr) {
        response->expires = parseCacheControl(mbgl::android::std_string_from_jstring(env, cacheControl).c_str());
    }

    if (expires != nullptr) {
        response->expires = util::parseTimePoint(mbgl::android::std_string_from_jstring(env, expires).c_str());
    }

    if (code == 200) {
        if (body != nullptr) {
            jbyte* bodyData = env->GetByteArrayElements(body, nullptr);
            response->data = std::make_shared<std::string>(reinterpret_cast<char*>(bodyData), env->GetArrayLength(body));
            env->ReleaseByteArrayElements(body, bodyData, JNI_ABORT);
        }
    } else if (code == 204 || (code == 404 && resource.kind == Resource::Kind::Tile)) {
        response->noContent = true;
    } else if (code == 304) {
        response->notModified = true;
    } else if (code == 404) {
        response->error = std::make_unique<Error>(Error::Reason::NotFound, "HTTP status code 404");
    } else if (code >= 500 && code < 600) {
        response->error = std::make_unique<Error>(Error::Reason::Server, std::string{ "HTTP status code " } + std::to_string(code));
    } else {
        response->error = std::make_unique<Error>(Error::Reason::Other, std::string{ "HTTP status code " } + std::to_string(code));
    }

    async.send();
}

void HTTPAndroidRequest::onFailure(JNIEnv* env, int type, jstring message) {
    std::string messageStr = mbgl::android::std_string_from_jstring(env, message);

    response = std::make_unique<Response>();
    using Error = Response::Error;

    switch (type) {
        case connectionError:
            response->error = std::make_unique<Error>(Error::Reason::Connection, messageStr);
            break;
        case temporaryError:
            response->error = std::make_unique<Error>(Error::Reason::Server, messageStr);
            break;
        case canceledError:
            response.reset();
            break;
        default:
            response->error = std::make_unique<Error>(Error::Reason::Other, messageStr);
    }

    async.send();
}

std::unique_ptr<HTTPContextBase> HTTPContextBase::createContext() {
    return std::make_unique<HTTPAndroidContext>();
}

void JNICALL nativeOnFailure(JNIEnv* env, jobject, jlong nativePtr, jint type, jstring message) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeOnFailure");
    assert(nativePtr != 0);
    return reinterpret_cast<HTTPAndroidRequest*>(nativePtr)->onFailure(env, type, message);
}

void JNICALL nativeOnResponse(JNIEnv* env, jobject, jlong nativePtr, jint code, jstring message, jstring etag, jstring modified, jstring cacheControl, jstring expires, jbyteArray body) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeOnResponse");
    assert(nativePtr != 0);
    return reinterpret_cast<HTTPAndroidRequest*>(nativePtr)->onResponse(env, code, message, etag, modified, cacheControl, expires, body);
}

}
