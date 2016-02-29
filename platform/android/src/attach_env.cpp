#include "attach_env.hpp"
#include "jni.hpp"

namespace mbgl {
namespace android {

UniqueEnv AttachEnv() {
    JNIEnv* env = nullptr;
    jint err = theJVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);

    switch (err) {
    case JNI_OK:
        return UniqueEnv(env, JNIEnvDeleter(*theJVM, false));
    case JNI_EDETACHED:
        return UniqueEnv(jni::AttachCurrentThread(*theJVM).release(), JNIEnvDeleter(*theJVM, true));
    default:
        throw std::system_error(err,  jni::ErrorCategory());
    }
}

}
}
