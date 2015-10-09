#include <mbgl/storage/http_context_base.hpp>
#include <mbgl/storage/http_request_base.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>
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
    explicit HTTPAndroidContext(uv_loop_t *loop);
    ~HTTPAndroidContext();

    HTTPRequestBase* createRequest(const Resource&,
                               RequestBase::Callback,
                               uv_loop_t*,
                               std::shared_ptr<const Response>) final;

    uv_loop_t *loop = nullptr;

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
    void retry() final;

    void onFailure(int type, std::string message);
    void onResponse(int code, std::string message, std::string etag, std::string modified, std::string cacheControl, std::string expires, std::string body);

private:
    void retry(uint64_t timeout) final;
    void finish();
    void start();

    HTTPAndroidContext *context = nullptr;

    bool cancelled = false;

    std::unique_ptr<Response> response;
    const std::shared_ptr<const Response> existingResponse;
    ResponseStatus status = ResponseStatus::PermanentError;

    jobject obj = nullptr;

    uv::async async;
    uv::timer timer;
    enum : bool { PreemptImmediately, ExponentialBackoff } strategy = PreemptImmediately;
    int attempts = 0;

    static const int maxAttempts = 4;

    static const int connectionError = 0;
    static const int temporaryError = 1;
    static const int permanentError = 2;
    static const int canceledError = 3;
};

// -------------------------------------------------------------------------------------------------

HTTPAndroidContext::HTTPAndroidContext(uv_loop_t *loop_)
    : HTTPContextBase(loop_),
      loop(loop_),
      vm(mbgl::android::theJVM) {

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
      async(loop, [this] { finish(); }),
      timer(loop) {

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

    mbgl::android::detach_jni_thread(context->vm, &env, detach);

    context->addRequest(this);
    start();
}

HTTPAndroidRequest::~HTTPAndroidRequest() {
    context->removeRequest(this);

    JNIEnv *env = nullptr;
    bool detach = mbgl::android::attach_jni_thread(context->vm, &env, "HTTPAndroidContext::~HTTPAndroidRequest()");

    env->DeleteGlobalRef(obj);
    obj = nullptr;

    mbgl::android::detach_jni_thread(context->vm, &env, detach);

    timer.stop();
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

void HTTPAndroidRequest::start() {
    attempts++;

    JNIEnv *env = nullptr;
    bool detach = mbgl::android::attach_jni_thread(context->vm, &env, "HTTPAndroidContext::start()");

    env->CallVoidMethod(obj, mbgl::android::httpRequestStartId);
    if (env->ExceptionCheck()) {
      env->ExceptionDescribe();
    }

    mbgl::android::detach_jni_thread(context->vm, &env, detach);
}

void HTTPAndroidRequest::retry(uint64_t timeout) {
    response.reset();

    timer.stop();
    timer.start(timeout, 0, [this] { start(); });
}

void HTTPAndroidRequest::retry() {
    if (strategy == PreemptImmediately) {
        timer.stop();
        timer.start(0, 0, [this] { start(); });
    }
}

void HTTPAndroidRequest::finish() {
    if (!cancelled) {
        if (status == ResponseStatus::TemporaryError && attempts < maxAttempts) {
            strategy = ExponentialBackoff;
            return retry((1 << (attempts - 1)) * 1000);
        } else if (status == ResponseStatus::ConnectionError && attempts < maxAttempts) {
            strategy = PreemptImmediately;
            return retry(30000);
        }

        if (status == ResponseStatus::NotModified) {
            notify(std::move(response), FileCache::Hint::Refresh);
        } else {
            notify(std::move(response), FileCache::Hint::Full);
        }
    }

    delete this;
}

int64_t parseCacheControl(const char *value) {
    if (value) {
        unsigned long long seconds = 0;
        // TODO: cache-control may contain other information as well:
        // http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.9
        if (std::sscanf(value, "max-age=%llu", &seconds) == 1) {
            return std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::system_clock::now().time_since_epoch()).count() +
                   seconds;
        }
    }

    return 0;
}

void HTTPAndroidRequest::onResponse(int code, std::string message, std::string etag, std::string modified, std::string cacheControl, std::string expires, std::string body) {
    if (!response) {
        response = std::make_unique<Response>();
    }

    response->message = message;
    response->modified = parse_date(modified.c_str());
    response->etag = etag;
    response->expires = parseCacheControl(cacheControl.c_str());
    if (!expires.empty()) {
        response->expires = parse_date(expires.c_str());
    }
    response->data = body;

    if (code == 304) {
        if (existingResponse) {
            response->status = existingResponse->status;
            response->message = existingResponse->message;
            response->modified = existingResponse->modified;
            response->etag = existingResponse->etag;
            response->data = existingResponse->data;
            status = ResponseStatus::NotModified;
        } else {
            response->status = Response::Successful;
            status = ResponseStatus::Successful;
        }
    } else if (code == 200) {
        response->status = Response::Successful;
        status = ResponseStatus::Successful;
    } else if (responseCode == 404) {
        response->status = Response::NotFound;
        status = ResponseStatus::Successful;
    } else if (code >= 500 && code < 600) {
        response->status = Response::Error;
        response->message = "HTTP status code " + util::toString(code);
        status = ResponseStatus::TemporaryError;
    } else {
        response->status = Response::Error;
        response->message = "HTTP status code " + util::toString(code);
        status = ResponseStatus::PermanentError;
    }

    async.send();
}

void HTTPAndroidRequest::onFailure(int type, std::string message) {
    if (type != canceledError) {
        if (!response) {
            response = std::make_unique<Response>();
        }

        response->status = Response::Error;
        response->message = message;

        switch (type) {
        case connectionError:
            status = ResponseStatus::ConnectionError;
            break;
        case temporaryError:
            status = ResponseStatus::TemporaryError;
            break;
        default:
            status = ResponseStatus::PermanentError;
        }
    } else {
        status = ResponseStatus::Canceled;
    }

    async.send();
}

std::unique_ptr<HTTPContextBase> HTTPContextBase::createContext(uv_loop_t* loop) {
    return std::make_unique<HTTPAndroidContext>(loop);
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
    jbyte* bodyData = env->GetByteArrayElements(body, nullptr);
    std::string bodyStr(reinterpret_cast<char*>(bodyData), env->GetArrayLength(body));
    env->ReleaseByteArrayElements(body, bodyData, JNI_ABORT);
    return request->onResponse(code, messageStr, etagStr, modifiedStr, cacheControlStr, expiresStr, bodyStr);
}

}
