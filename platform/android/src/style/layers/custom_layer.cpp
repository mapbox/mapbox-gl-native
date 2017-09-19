#include "custom_layer.hpp"

#include <string>

#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace android {

    CustomLayer::CustomLayer(jni::JNIEnv& env, jni::String layerId, jni::jlong initializeFunction, jni::jlong renderFunction, jni::jlong contextLostFunction, jni::jlong deinitializeFunction, jni::jlong context)
        : Layer(env, std::make_unique<mbgl::style::CustomLayer>(
                jni::Make<std::string>(env, layerId),
                reinterpret_cast<mbgl::style::CustomLayerInitializeFunction>(initializeFunction),
                reinterpret_cast<mbgl::style::CustomLayerRenderFunction>(renderFunction),
                reinterpret_cast<mbgl::style::CustomLayerContextLostFunction>(contextLostFunction),
                reinterpret_cast<mbgl::style::CustomLayerDeinitializeFunction>(deinitializeFunction),
                reinterpret_cast<void*>(context))
                ) {
    }

    CustomLayer::CustomLayer(mbgl::Map& map, mbgl::style::CustomLayer& coreLayer)
        : Layer(map, coreLayer) {
    }

    CustomLayer::CustomLayer(mbgl::Map& map, std::unique_ptr<mbgl::style::CustomLayer> coreLayer)
            : Layer(map, std::move(coreLayer)) {
    }

    CustomLayer::~CustomLayer() = default;

    void CustomLayer::update(jni::JNIEnv&) {
        Log::Debug(mbgl::Event::JNI, "Updating map");
        if (map) {
            map->triggerRepaint();
        } else {
            Log::Error(mbgl::Event::JNI, "No map reference, cannot update");
        }
    }

    jni::Class<CustomLayer> CustomLayer::javaClass;

    jni::jobject* CustomLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = CustomLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return CustomLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void CustomLayer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        CustomLayer::javaClass = *jni::Class<CustomLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<CustomLayer>(
            env, CustomLayer::javaClass, "nativePtr",
            std::make_unique<CustomLayer, JNIEnv&, jni::String, jni::jlong, jni::jlong, jni::jlong, jni::jlong, jni::jlong>,
            "initialize",
            "finalize",
            METHOD(&CustomLayer::update, "nativeUpdate"));
    }

} // namespace android
} // namespace mbgl
