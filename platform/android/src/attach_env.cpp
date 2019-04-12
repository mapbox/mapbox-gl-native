#include <android/log.h>
#include <mbgl/util/platform.hpp>
#include "attach_env.hpp"
#include "jni.hpp"

namespace mbgl {
namespace android {

UniqueEnv AttachEnv() {
    JNIEnv* env = nullptr;
    jint err = theJVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);

    switch (err) {
    case JNI_OK:
        __android_log_write(ANDROID_LOG_ERROR, "JNI_THREAD", ("is attached: " + mbgl::platform::getCurrentThreadName()).c_str());
        return UniqueEnv(env, JNIEnvDeleter(*theJVM, false));
    case JNI_EDETACHED:
        __android_log_write(ANDROID_LOG_ERROR, "JNI_THREAD", ("is detached: " + mbgl::platform::getCurrentThreadName()).c_str());
        return UniqueEnv(jni::AttachCurrentThread(*theJVM).release(), JNIEnvDeleter(*theJVM, true));
    default:
        throw std::system_error(err,  jni::ErrorCategory());
    }
}

}
}
