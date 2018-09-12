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

    jni::Local<jni::Object<Source>> UnknownSource::createJavaPeer(jni::JNIEnv& env) {
        static auto& javaClass = jni::Class<UnknownSource>::Singleton(env);
        static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
        return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void UnknownSource::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto& javaClass = jni::Class<UnknownSource>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<UnknownSource>(
            env, javaClass, "nativePtr",
            init,
            "initialize",
            "finalize"
        );
    }

} // namespace android
} // namespace mbgl
