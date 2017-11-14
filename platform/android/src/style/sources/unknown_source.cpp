#include "unknown_source.hpp"

namespace {

    // Dummy initializer (We don't support initializing this from the JVM)
    std::unique_ptr<mbgl::android::UnknownSource> init(jni::JNIEnv&) {
        throw std::runtime_error("UnknownSource should not be initialized from the JVM");
    }

} // namespace

namespace mbgl {
namespace android {

    UnknownSource::UnknownSource(jni::JNIEnv& env,
                                 mbgl::style::Source& coreSource,
                                 AndroidRendererFrontend& frontend)
        : Source(env, coreSource, createJavaPeer(env), frontend) {
    }

    jni::Class<UnknownSource> UnknownSource::javaClass;

    jni::Object<Source> UnknownSource::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = UnknownSource::javaClass.template GetConstructor<jni::jlong>(env);
        return jni::Object<Source>(UnknownSource::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this)).Get());
    }

    void UnknownSource::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        UnknownSource::javaClass = *jni::Class<UnknownSource>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<UnknownSource>(
            env, UnknownSource::javaClass, "nativePtr",
            init,
            "initialize",
            "finalize"
        );
    }

} // namespace android
} // namespace mbgl
