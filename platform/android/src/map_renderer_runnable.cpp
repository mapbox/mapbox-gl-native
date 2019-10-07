#include "map_renderer_runnable.hpp"

#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace android {

MapRendererRunnable::MapRendererRunnable(jni::JNIEnv& env, std::function<void()> function_)
    : function(std::move(function_)) {
    // Create the Java peer and hold on to a global reference
    // Not using a weak reference here as this might oerflow
    // the weak reference table on some devices
    jni::UniqueLocalFrame frame = jni::PushLocalFrame(env, 5);
    static auto& javaClass = jni::Class<MapRendererRunnable>::Singleton(env);
    static auto constructor = javaClass.GetConstructor<jlong>(env);
    auto instance = javaClass.New(env, constructor, reinterpret_cast<jlong>(this));
    javaPeer = jni::NewGlobal(env, instance);
}

MapRendererRunnable::~MapRendererRunnable() = default;

void MapRendererRunnable::run(jni::JNIEnv&) {
    if (function) function();
}

jni::Global<jni::Object<MapRendererRunnable>> MapRendererRunnable::peer() {
    return std::move(javaPeer);
}

// Static methods //

void MapRendererRunnable::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    static auto& javaClass = jni::Class<MapRendererRunnable>::Singleton(env);

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    jni::RegisterNativePeer<MapRendererRunnable>(
            env,
            javaClass,
            "nativePtr",
            std::make_unique<MapRendererRunnable, JNIEnv&>,
            "nativeInitialize",
            "finalize",
            METHOD(&MapRendererRunnable::run, "run"));
}

} // namespace android
} // namespace mbgl
