#include "unknown_layer.hpp"

#include <string>

namespace {

    // Dummy initializer (We don't support initializing this from the JVM)
    std::unique_ptr<mbgl::android::UnknownLayer> init(jni::JNIEnv&) {
        throw std::runtime_error("UnknownLayer should not be initialized from the JVM");
    }

} // namespace

namespace mbgl {
namespace android {

    UnknownLayer::UnknownLayer(mbgl::Map& map, mbgl::style::Layer& coreLayer)
        : Layer(map, coreLayer) {
    }

    UnknownLayer::UnknownLayer(mbgl::Map& map, std::unique_ptr<mbgl::style::Layer> coreLayer)
        : Layer(map, std::move(coreLayer)) {
    }

    jni::Class<UnknownLayer> UnknownLayer::javaClass;

    jni::jobject* UnknownLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = UnknownLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return UnknownLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void UnknownLayer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        UnknownLayer::javaClass = *jni::Class<UnknownLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        std::function<std::unique_ptr<UnknownLayer>(JNIEnv&)> initializer = nullptr;

        // Register the peer
        jni::RegisterNativePeer<UnknownLayer>(
            env, UnknownLayer::javaClass, "nativePtr",
            init,
            "initialize",
            "finalize");
    }

} // namespace android
} // namespace mbgl
