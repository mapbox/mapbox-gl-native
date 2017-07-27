#include "offline_region_error.hpp"

namespace mbgl {
namespace android {

jni::Object<OfflineRegionError> OfflineRegionError::New(jni::JNIEnv& env, mbgl::Response::Error error) {

    // Handle the value of reason independently of the underlying int value
    std::string reason;
    switch(error.status) {
        case mbgl::ResourceStatus::Success:
            reason = "REASON_SUCCESS";
            break;
        case mbgl::ResourceStatus::NotFoundError:
            reason = "REASON_NOT_FOUND";
            break;
        case mbgl::ResourceStatus::ServerError:
            reason = "REASON_SERVER";
            break;
        case mbgl::ResourceStatus::ConnectionError:
            reason = "REASON_CONNECTION";
            break;
        case mbgl::ResourceStatus::RateLimitError:
            reason = "REASON_RATE_LIMIT";
            break;
        case mbgl::ResourceStatus::OtherError:
            reason = "REASON_OTHER";
            break;
    }

    // Convert
    auto jReason = jni::Make<jni::String>(env, reason);
    auto jMessage = jni::Make<jni::String>(env, error.message);

    // Create java object
    static auto constructor = javaClass.GetConstructor<jni::String, jni::String>(env);
    auto jError = javaClass.New(env, constructor, jReason, jMessage);

    // Delete references
    jni::DeleteLocalRef(env, jReason);
    jni::DeleteLocalRef(env, jMessage);

    return jError;
}

jni::Class<OfflineRegionError> OfflineRegionError::javaClass;

void OfflineRegionError::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<OfflineRegionError>::Find(env).NewGlobalRef(env).release();
}

} // namespace android
} // namespace mbgl
