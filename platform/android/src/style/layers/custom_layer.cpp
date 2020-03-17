#include "custom_layer.hpp"

#include <string>

#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace android {

    CustomLayer::CustomLayer(jni::JNIEnv& env, const jni::String& layerId, jni::jlong host)
        : Layer(std::make_unique<mbgl::style::CustomLayer>(
                jni::Make<std::string>(env, layerId),
                std::unique_ptr<mbgl::style::CustomLayerHost>(reinterpret_cast<mbgl::style::CustomLayerHost*>(host)))
                ) {
    }

    CustomLayer::CustomLayer(mbgl::style::CustomLayer& coreLayer) : Layer(coreLayer) {}

    CustomLayer::CustomLayer(std::unique_ptr<mbgl::style::CustomLayer> coreLayer) : Layer(std::move(coreLayer)) {}

    CustomLayer::~CustomLayer() = default;

    namespace {
        jni::Local<jni::Object<Layer>> createJavaPeer(jni::JNIEnv& env, Layer* layer) {
            static auto& javaClass = jni::Class<CustomLayer>::Singleton(env);
            static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
            return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(layer));
        }
    }

    CustomJavaLayerPeerFactory::~CustomJavaLayerPeerFactory() = default;

    jni::Local<jni::Object<Layer>> CustomJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env,
                                                                                   mbgl::style::Layer& layer) {
        return createJavaPeer(env, new CustomLayer(static_cast<mbgl::style::CustomLayer&>(layer)));
    }

    jni::Local<jni::Object<Layer>> CustomJavaLayerPeerFactory::createJavaLayerPeer(
        jni::JNIEnv& env, std::unique_ptr<mbgl::style::Layer> layer) {
        return createJavaPeer(env,
                              new CustomLayer(std::unique_ptr<mbgl::style::CustomLayer>(
                                  static_cast<mbgl::style::CustomLayer*>(layer.release()))));
    }

    void CustomJavaLayerPeerFactory::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto& javaClass = jni::Class<CustomLayer>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<CustomLayer>(env,
                                             javaClass,
                                             "nativePtr",
                                             jni::MakePeer<CustomLayer, const jni::String&, jni::jlong>,
                                             "initialize",
                                             "finalize");
    }

} // namespace android
} // namespace mbgl
