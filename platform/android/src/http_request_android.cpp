#include <mbgl/storage/http_context_base.hpp>
#include <mbgl/storage/http_request_base.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/platform/log.hpp>

#include <mbgl/util/async_task.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/string.hpp>

#include <jni/jni.hpp>
#include "attach_env.hpp"

namespace mbgl {
namespace android {

class HTTPContext : public HTTPContextBase {
public:
    HTTPRequestBase* createRequest(const Resource&, HTTPRequestBase::Callback) final;
    UniqueEnv env { android::AttachEnv() };
};

class HTTPRequest : public HTTPRequestBase {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/http/HTTPRequest"; };

    HTTPRequest(jni::JNIEnv&, const Resource&, Callback);

    void cancel() final;

    void onFailure(jni::JNIEnv&, int type, jni::String message);
    void onResponse(jni::JNIEnv&, int code,
                    jni::String etag, jni::String modified,
                    jni::String cacheControl, jni::String expires,
                    jni::Array<jni::jbyte> body);

    static jni::Class<HTTPRequest> javaClass;
    jni::UniqueObject<HTTPRequest> javaRequest;

private:
    void finish();

    std::unique_ptr<Response> response;
    const std::shared_ptr<const Response> existingResponse;

    util::AsyncTask async;

    static const int connectionError = 0;
    static const int temporaryError = 1;
    static const int permanentError = 2;
};

jni::Class<HTTPRequest> HTTPRequest::javaClass;

void RegisterNativeHTTPRequest(jni::JNIEnv& env) {
    HTTPRequest::javaClass = *jni::Class<HTTPRequest>::Find(env).NewGlobalRef(env).release();

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    jni::RegisterNativePeer<HTTPRequest>(env, HTTPRequest::javaClass, "mNativePtr",
        METHOD(&HTTPRequest::onFailure, "nativeOnFailure"),
        METHOD(&HTTPRequest::onResponse, "nativeOnResponse"));
}

// -------------------------------------------------------------------------------------------------

HTTPRequestBase* HTTPContext::createRequest(const Resource& resource, HTTPRequestBase::Callback callback) {
    return new HTTPRequest(*env, resource, callback);
}

HTTPRequest::HTTPRequest(jni::JNIEnv& env, const Resource& resource_, Callback callback_)
    : HTTPRequestBase(resource_, callback_),
      async([this] { finish(); }) {
    std::string etagStr;
    std::string modifiedStr;

    if (resource.priorEtag) {
        etagStr = *resource.priorEtag;
    } else if (resource.priorModified) {
        modifiedStr = util::rfc1123(*resource.priorModified);
    }

    jni::UniqueLocalFrame frame = jni::PushLocalFrame(env, 10);

    static auto constructor =
        javaClass.GetConstructor<jni::jlong, jni::String, jni::String, jni::String, jni::String>(env);

    javaRequest = javaClass.New(env, constructor,
        reinterpret_cast<jlong>(this),
        jni::Make<jni::String>(env, resource.url),
        jni::Make<jni::String>(env, "MapboxGL/1.0"),
        jni::Make<jni::String>(env, etagStr),
        jni::Make<jni::String>(env, modifiedStr)).NewGlobalRef(env);
}

void HTTPRequest::cancel() {
    UniqueEnv env = android::AttachEnv();

    static auto cancel = javaClass.GetMethod<void ()>(*env, "cancel");

    javaRequest->Call(*env, cancel);

    delete this;
}

void HTTPRequest::finish() {
    assert(response);
    notify(*response);

    delete this;
}

void HTTPRequest::onResponse(jni::JNIEnv& env, int code,
                             jni::String etag, jni::String modified, jni::String cacheControl,
                             jni::String expires, jni::Array<jni::jbyte> body) {
    response = std::make_unique<Response>();
    using Error = Response::Error;

    if (etag) {
        response->etag = jni::Make<std::string>(env, etag);
    }

    if (modified) {
        response->modified = util::parseTimePoint(jni::Make<std::string>(env, modified).c_str());
    }

    if (cacheControl) {
        response->expires = parseCacheControl(jni::Make<std::string>(env, cacheControl).c_str());
    }

    if (expires) {
        response->expires = util::parseTimePoint(jni::Make<std::string>(env, expires).c_str());
    }

    if (code == 200) {
        if (body) {
            auto data = std::make_shared<std::string>(body.Length(env), char());
            jni::GetArrayRegion(env, *body, 0, data->size(), reinterpret_cast<jbyte*>(&(*data)[0]));
            response->data = data;
        } else {
            response->data = std::make_shared<std::string>();
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

void HTTPRequest::onFailure(jni::JNIEnv& env, int type, jni::String message) {
    std::string messageStr = jni::Make<std::string>(env, message);

    response = std::make_unique<Response>();
    using Error = Response::Error;

    switch (type) {
        case connectionError:
            response->error = std::make_unique<Error>(Error::Reason::Connection, messageStr);
            break;
        case temporaryError:
            response->error = std::make_unique<Error>(Error::Reason::Server, messageStr);
            break;
        default:
            response->error = std::make_unique<Error>(Error::Reason::Other, messageStr);
    }

    async.send();
}

} // namespace android

std::unique_ptr<HTTPContextBase> HTTPContextBase::createContext() {
    return std::make_unique<android::HTTPContext>();
}

uint32_t HTTPContextBase::maximumConcurrentRequests() {
    return 20;
}

} // namespace mbgl
