#include <jni/jni.hpp>
#include <mbgl/test.hpp>

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    struct Main { static constexpr auto Name() { return "Main"; } };

    jni::JNIEnv& env = jni::GetEnv(*vm, jni::jni_version_1_6);
    jni::RegisterNatives(env, jni::Class<Main>::Find(env),
         jni::MakeNativeMethod("run", [] { mbgl::runTests(0, nullptr); }));

    return JNI_VERSION_1_6;
}
