#include "map_renderer_runnable.hpp"

#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace android {

MapRendererRunnable::MapRendererRunnable(jni::JNIEnv& env, std::weak_ptr<Mailbox> mailbox_)
    : mailbox(std::move(mailbox_)) {

    // Create the Java peer and hold on to a global reference
    // Not using a weak reference here as this might oerflow
    // the weak reference table on some devices
    jni::UniqueLocalFrame frame = jni::PushLocalFrame(env, 5);
    static auto constructor = javaClass.GetConstructor<jlong>(env);
    auto instance = javaClass.New(env, constructor, reinterpret_cast<jlong>(this));
    javaPeer = instance.NewGlobalRef(env);
}

MapRendererRunnable::~MapRendererRunnable() = default;

void MapRendererRunnable::run(jni::JNIEnv&) {
    Mailbox::maybeReceive(mailbox);
}

jni::UniqueObject<MapRendererRunnable> MapRendererRunnable::peer() {
    return std::move(javaPeer);
}

// Static methods //

jni::Class<MapRendererRunnable> MapRendererRunnable::javaClass;

void MapRendererRunnable::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    MapRendererRunnable::javaClass = *jni::Class<MapRendererRunnable>::Find(env).NewGlobalRef(env).release();

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    jni::RegisterNativePeer<MapRendererRunnable>(
            env,
            MapRendererRunnable::javaClass,
            "nativePtr",
            std::make_unique<MapRendererRunnable, JNIEnv&>,
            "nativeInitialize",
            "finalize",
            METHOD(&MapRendererRunnable::run, "run"));
}

} // namespace android
} // namespace mbgl
