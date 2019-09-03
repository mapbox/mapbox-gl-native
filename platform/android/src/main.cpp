#include "jni.hpp"
#include "jni_native.hpp"
#include <jni/jni.hpp>

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    assert(vm != nullptr);
    mbgl::android::registerNatives(vm);
    return JNI_VERSION_1_6;
}
