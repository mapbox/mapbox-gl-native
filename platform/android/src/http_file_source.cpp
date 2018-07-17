#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/logging.hpp>

#include <mbgl/util/async_task.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/http_header.hpp>

#include <jni/jni.hpp>
#include "attach_env.hpp"

namespace mbgl {

class HTTPFileSource::Impl {
public:
    android::UniqueEnv env { android::AttachEnv() };
};

class HTTPRequest : public AsyncRequest {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/http/NativeHttpRequest"; };

    HTTPRequest(jni::JNIEnv&, const Resource&, FileSource::Callback);
    ~HTTPRequest();

    void onFailure(jni::JNIEnv&, int type, jni::String message);
    void onResponse(jni::JNIEnv&, int code,
                    jni::String etag, jni::String modified,
                    jni::String cacheControl, jni::String expires,
                    jni::String retryAfter, jni::String xRateLimitReset,
                    jni::Array<jni::jbyte> body);

    static jni::Class<HTTPRequest> javaClass;
    jni::UniqueObject<HTTPRequest> javaRequest;

private:
    Resource resource;
    FileSource::Callback callback;
    Response response;

    util::AsyncTask async { [this] {
        // Calling `callback` may result in deleting `this`. Copy data to temporaries first.
        auto callback_ = callback;
        auto response_ = response;
        callback_(response_);
    } };

    static const int connectionError = 0;
    static const int temporaryError = 1;
    static const int permanentError = 2;
};

jni::Class<HTTPRequest> HTTPRequest::javaClass;

namespace android {

void RegisterNativeHTTPRequest(jni::JNIEnv& env) {
    HTTPRequest::javaClass = *jni::Class<HTTPRequest>::Find(env).NewGlobalRef(env).release();

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    jni::RegisterNativePeer<HTTPRequest>(env, HTTPRequest::javaClass, "nativePtr",
        METHOD(&HTTPRequest::onFailure, "nativeOnFailure"),
        METHOD(&HTTPRequest::onResponse, "nativeOnResponse"));
}

} // namespace android

HTTPRequest::HTTPRequest(jni::JNIEnv& env, const Resource& resource_, FileSource::Callback callback_)
    : resource(resource_),
      callback(callback_) {
    std::string etagStr;
    std::string modifiedStr;

    if (resource.priorEtag) {
        etagStr = *resource.priorEtag;
    } else if (resource.priorModified) {
        modifiedStr = util::rfc1123(*resource.priorModified);
    }

    jni::UniqueLocalFrame frame = jni::PushLocalFrame(env, 10);

    static auto constructor =
        javaClass.GetConstructor<jni::jlong, jni::String, jni::String, jni::String>(env);

    javaRequest = javaClass.New(env, constructor,
        reinterpret_cast<jlong>(this),
        jni::Make<jni::String>(env, resource.url),
        jni::Make<jni::String>(env, etagStr),
        jni::Make<jni::String>(env, modifiedStr)).NewGlobalRef(env);
}

HTTPRequest::~HTTPRequest() {
    android::UniqueEnv env = android::AttachEnv();

    static auto cancel = javaClass.GetMethod<void ()>(*env, "cancel");

    javaRequest->Call(*env, cancel);
}

void HTTPRequest::onResponse(jni::JNIEnv& env, int code,
                             jni::String etag, jni::String modified,
                             jni::String cacheControl, jni::String expires,
                             jni::String jRetryAfter, jni::String jXRateLimitReset,
                             jni::Array<jni::jbyte> body) {

    using Error = Response::Error;

    if (etag) {
        response.etag = jni::Make<std::string>(env, etag);
    }

    if (modified) {
        response.modified = util::parseTimestamp(jni::Make<std::string>(env, modified).c_str());
    }

    if (cacheControl) {
        const auto cc = http::CacheControl::parse(jni::Make<std::string>(env, cacheControl).c_str());
        response.expires = cc.toTimePoint();
        response.mustRevalidate = cc.mustRevalidate;
    }

    if (expires) {
        response.expires = util::parseTimestamp(jni::Make<std::string>(env, expires).c_str());
    }

    if (code == 200) {
        if (body) {
            auto data = std::make_shared<std::string>(body.Length(env), char());
            jni::GetArrayRegion(env, *body, 0, data->size(), reinterpret_cast<jbyte*>(&(*data)[0]));
            response.data = data;
        } else {
            response.data = std::make_shared<std::string>();
        }
    } else if (code == 204 || (code == 404 && resource.kind == Resource::Kind::Tile)) {
        response.noContent = true;
    } else if (code == 304) {
        response.notModified = true;
    } else if (code == 404) {
        response.error = std::make_unique<Error>(Error::Reason::NotFound, "HTTP status code 404");
    } else if (code == 429) {
        optional<std::string> retryAfter;
        optional<std::string> xRateLimitReset;
        if (jRetryAfter) {
            retryAfter = jni::Make<std::string>(env, jRetryAfter);
        }
        if (jXRateLimitReset) {
            xRateLimitReset = jni::Make<std::string>(env, jXRateLimitReset);
        }
        response.error = std::make_unique<Error>(Error::Reason::RateLimit, "HTTP status code 429", http::parseRetryHeaders(retryAfter, xRateLimitReset));
    } else if (code >= 500 && code < 600) {
        response.error = std::make_unique<Error>(Error::Reason::Server, std::string{ "HTTP status code " } + std::to_string(code));
    } else {
        response.error = std::make_unique<Error>(Error::Reason::Other, std::string{ "HTTP status code " } + std::to_string(code));
    }

    async.send();
}

void HTTPRequest::onFailure(jni::JNIEnv& env, int type, jni::String message) {
    std::string messageStr = jni::Make<std::string>(env, message);

    using Error = Response::Error;

    switch (type) {
        case connectionError:
            response.error = std::make_unique<Error>(Error::Reason::Connection, messageStr);
            break;
        case temporaryError:
            response.error = std::make_unique<Error>(Error::Reason::Server, messageStr);
            break;
        default:
            response.error = std::make_unique<Error>(Error::Reason::Other, messageStr);
    }

    async.send();
}

HTTPFileSource::HTTPFileSource()
    : impl(std::make_unique<Impl>()) {
}

HTTPFileSource::~HTTPFileSource() = default;

std::unique_ptr<AsyncRequest> HTTPFileSource::request(const Resource& resource, Callback callback) {
    return std::make_unique<HTTPRequest>(*impl->env, resource, callback);
}

uint32_t HTTPFileSource::maximumConcurrentRequests() {
    return 20;
}

} // namespace mbgl
