#include <mbgl/storage/http_context_base.hpp>
#include <mbgl/storage/http_request_base.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/android/jni.hpp>

#include <mbgl/util/time.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/parsedate.h>

#include <jni.h>

namespace mbgl {

void JNICALL nativeOnFailure(JNIEnv *env, jobject obj, jlong nativePtr, jint type, jstring message);
void JNICALL nativeOnResponse(JNIEnv *env, jobject obj, jlong nativePtr, jint code, jstring message, jstring etag, jstring modified, jstring cacheControl, jstring expires, jbyteArray body);

class HTTPAndroidRequest;

class HTTPAndroidContext : public HTTPContextBase {
public:
    explicit HTTPAndroidContext();
    ~HTTPAndroidContext();

    HTTPRequestBase* createRequest(const Resource&,
                               RequestBase::Callback,
                               uv_loop_t*,
                               std::shared_ptr<const Response>) final;

    JavaVM *vm = nullptr;
    jobject obj = nullptr;
};

class HTTPAndroidRequest : public HTTPRequestBase {
public:
    HTTPAndroidRequest(HTTPAndroidContext*,
                const Resource&,
                Callback,
                uv_loop_t*,
                std::shared_ptr<const Response>);
    ~HTTPAndroidRequest();

    void cancel() final;

    void onFailure(int type, std::string message);
    void onResponse(int code, std::string message, std::string etag, std::string modified, std::string cacheControl, std::string expires, std::string body);

private:
    void finish();

    HTTPAndroidContext *context = nullptr;

    bool cancelled = false;

    std::unique_ptr<Response> response;
    const std::shared_ptr<const Response> existingResponse;

    jobject obj = nullptr;

    uv::async async;

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

HTTPRequestBase* HTTPAndroidContext::createRequest(const Resource& resource,
                                            RequestBase::Callback callback,
                                            uv_loop_t* loop_,
                                            std::shared_ptr<const Response> response) {
    return new HTTPAndroidRequest(this, resource, callback, loop_, response);
}

HTTPAndroidRequest::HTTPAndroidRequest(HTTPAndroidContext* context_, const Resource& resource_, Callback callback_, uv_loop_t* loop, std::shared_ptr<const Response> response_)
    : HTTPRequestBase(resource_, callback_),
      context(context_),
      existingResponse(response_),
      async(loop, [this] { finish(); }) {

    std::string etagStr;
    std::string modifiedStr;
    if (existingResponse) {
        if (!existingResponse->etag.empty()) {
            etagStr = existingResponse->etag;
        } else if (existingResponse->modified) {
            modifiedStr = util::rfc1123(existingResponse->modified);
        }
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
        notify(std::move(response));
    }

    delete this;
}

void HTTPAndroidRequest::onResponse(int code, std::string message, std::string etag, std::string modified, std::string cacheControl, std::string expires, std::string body) {
    response = std::make_unique<Response>();
    using Error = Response::Error;

    response->modified = parse_date(modified.c_str());
    response->etag = etag;
    response->expires = parseCacheControl(cacheControl.c_str());
    if (!expires.empty()) {
        response->expires = parse_date(expires.c_str());
    }
    response->data = std::make_shared<std::string>(body);

    if (code == 200) {
        // Nothing to do; this is what we want
    } else if (code == 304) {
        if (existingResponse) {
            if (existingResponse->error) {
                response->error = std::make_unique<Error>(*existingResponse->error);
            }
            response->data = existingResponse->data;
            response->modified = existingResponse->modified;
            // We're not updating `expired`, it was probably set during the request.
            response->etag = existingResponse->etag;
        } else {
            // This is an unsolicited 304 response and should only happen on malfunctioning
            // HTTP servers. It likely doesn't include any data, but we don't have much options.
        }
    } else if (code == 404) {
        response->error = std::make_unique<Error>(Error::Reason::NotFound, "HTTP status code 404");
    } else if (code >= 500 && code < 600) {
        response->error = std::make_unique<Error>(Error::Reason::Server, std::string{ "HTTP status code " } + std::to_string(code));
    } else {
        response->error = std::make_unique<Error>(Error::Reason::Other, std::string{ "HTTP status code " } + std::to_string(code));
    }

    async.send();
}

void HTTPAndroidRequest::onFailure(int type, std::string message) {
    response = std::make_unique<Response>();
    using Error = Response::Error;

    switch (type) {
        case connectionError:
            response->error = std::make_unique<Error>(Error::Reason::Connection, message);
            break;
        case temporaryError:
            response->error = std::make_unique<Error>(Error::Reason::Server, message);
            break;
        case canceledError:
            response->error = std::make_unique<Error>(Error::Reason::Canceled, "Request was cancelled");
            break;
        default:
            response->error = std::make_unique<Error>(Error::Reason::Other, message);
    }

    async.send();
}

std::unique_ptr<HTTPContextBase> HTTPContextBase::createContext(uv_loop_t* loop) {
    return std::make_unique<HTTPAndroidContext>();
}

#pragma clang diagnostic ignored "-Wunused-parameter"

void JNICALL nativeOnFailure(JNIEnv *env, jobject obj, jlong nativePtr, jint type, jstring message) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeOnFailure");
    assert(nativePtr != 0);
    HTTPAndroidRequest *request = reinterpret_cast<HTTPAndroidRequest *>(nativePtr);
    std::string messageStr = mbgl::android::std_string_from_jstring(env, message);
    return request->onFailure(type, messageStr);
}

void JNICALL nativeOnResponse(JNIEnv *env, jobject obj, jlong nativePtr, jint code, jstring message, jstring etag, jstring modified, jstring cacheControl, jstring expires, jbyteArray body) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeOnResponse");
    assert(nativePtr != 0);
    HTTPAndroidRequest *request = reinterpret_cast<HTTPAndroidRequest *>(nativePtr);
    std::string messageStr = mbgl::android::std_string_from_jstring(env, message);
    std::string etagStr;
    if (etag != nullptr) {
        etagStr = mbgl::android::std_string_from_jstring(env, etag);
    }
    std::string modifiedStr;
    if (modified != nullptr) {
        modifiedStr = mbgl::android::std_string_from_jstring(env, modified);
    }
    std::string cacheControlStr;
    if (cacheControl != nullptr) {
        cacheControlStr = mbgl::android::std_string_from_jstring(env, cacheControl);
    }
    std::string expiresStr;
    if (expires != nullptr) {
        expiresStr = mbgl::android::std_string_from_jstring(env, expires);
    }
    std::string bodyStr;
    if (body != nullptr) {
        jbyte* bodyData = env->GetByteArrayElements(body, nullptr);
        bodyStr = std::string(reinterpret_cast<char*>(bodyData), env->GetArrayLength(body));
        env->ReleaseByteArrayElements(body, bodyData, JNI_ABORT);
    }
    return request->onResponse(code, messageStr, etagStr, modifiedStr, cacheControlStr, expiresStr, bodyStr);
}

}
