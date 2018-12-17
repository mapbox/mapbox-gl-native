// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "heatmap_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace android {

    inline mbgl::style::HeatmapLayer& toHeatmapLayer(mbgl::style::Layer& layer) {
        return static_cast<mbgl::style::HeatmapLayer&>(layer);
    }

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    HeatmapLayer::HeatmapLayer(jni::JNIEnv& env, jni::String& layerId, jni::String& sourceId)
        : Layer(std::make_unique<mbgl::style::HeatmapLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
    }

    /**
     * Creates a non-owning peer object (for layers currently attached to the map)
     */
    HeatmapLayer::HeatmapLayer(mbgl::Map& map, mbgl::style::HeatmapLayer& coreLayer)
        : Layer(map, coreLayer) {
    }

    /**
     * Creates an owning peer object (for layers not attached to the map)
     */
    HeatmapLayer::HeatmapLayer(mbgl::Map& map, std::unique_ptr<mbgl::style::HeatmapLayer> coreLayer)
        : Layer(map, std::move(coreLayer)) {
    }

    HeatmapLayer::~HeatmapLayer() = default;

    // Property getters

    jni::Local<jni::Object<>> HeatmapLayer::getHeatmapRadius(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toHeatmapLayer(layer).getHeatmapRadius()));
    }

    jni::Local<jni::Object<TransitionOptions>> HeatmapLayer::getHeatmapRadiusTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toHeatmapLayer(layer).getHeatmapRadiusTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void HeatmapLayer::setHeatmapRadiusTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toHeatmapLayer(layer).setHeatmapRadiusTransition(options);
    }

    jni::Local<jni::Object<>> HeatmapLayer::getHeatmapWeight(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toHeatmapLayer(layer).getHeatmapWeight()));
    }

    jni::Local<jni::Object<>> HeatmapLayer::getHeatmapIntensity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toHeatmapLayer(layer).getHeatmapIntensity()));
    }

    jni::Local<jni::Object<TransitionOptions>> HeatmapLayer::getHeatmapIntensityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toHeatmapLayer(layer).getHeatmapIntensityTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void HeatmapLayer::setHeatmapIntensityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toHeatmapLayer(layer).setHeatmapIntensityTransition(options);
    }

    jni::Local<jni::Object<>> HeatmapLayer::getHeatmapColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        auto propertyValue =  toHeatmapLayer(layer).getHeatmapColor();
        if (propertyValue.isUndefined()) {
            propertyValue =  toHeatmapLayer(layer).getDefaultHeatmapColor();
        }
        return std::move(*convert<jni::Local<jni::Object<>>>(env, propertyValue));
    }

    jni::Local<jni::Object<>> HeatmapLayer::getHeatmapOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toHeatmapLayer(layer).getHeatmapOpacity()));
    }

    jni::Local<jni::Object<TransitionOptions>> HeatmapLayer::getHeatmapOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toHeatmapLayer(layer).getHeatmapOpacityTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void HeatmapLayer::setHeatmapOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toHeatmapLayer(layer).setHeatmapOpacityTransition(options);
    }


    // HeatmapJavaLayerPeerFactory

    HeatmapJavaLayerPeerFactory::~HeatmapJavaLayerPeerFactory() = default;

    namespace {
        jni::Local<jni::Object<Layer>> createJavaPeer(jni::JNIEnv& env, Layer* layer) {
            static auto& javaClass = jni::Class<HeatmapLayer>::Singleton(env);
            static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
            return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(layer));
        }
    }  // namespace

    jni::Local<jni::Object<Layer>> HeatmapJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, mbgl::style::Layer& layer) {
        assert(layer.baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new HeatmapLayer(map, toHeatmapLayer(layer)));
    }

    jni::Local<jni::Object<Layer>> HeatmapJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer> layer) {
        assert(layer->baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new HeatmapLayer(map, std::unique_ptr<mbgl::style::HeatmapLayer>(static_cast<mbgl::style::HeatmapLayer*>(layer.release()))));
    }

    void HeatmapJavaLayerPeerFactory::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto& javaClass = jni::Class<HeatmapLayer>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<HeatmapLayer>(
            env, javaClass, "nativePtr",
            jni::MakePeer<HeatmapLayer, jni::String&, jni::String&>,
            "initialize",
            "finalize",
            METHOD(&HeatmapLayer::getHeatmapRadiusTransition, "nativeGetHeatmapRadiusTransition"),
            METHOD(&HeatmapLayer::setHeatmapRadiusTransition, "nativeSetHeatmapRadiusTransition"),
            METHOD(&HeatmapLayer::getHeatmapRadius, "nativeGetHeatmapRadius"),
            METHOD(&HeatmapLayer::getHeatmapWeight, "nativeGetHeatmapWeight"),
            METHOD(&HeatmapLayer::getHeatmapIntensityTransition, "nativeGetHeatmapIntensityTransition"),
            METHOD(&HeatmapLayer::setHeatmapIntensityTransition, "nativeSetHeatmapIntensityTransition"),
            METHOD(&HeatmapLayer::getHeatmapIntensity, "nativeGetHeatmapIntensity"),
            METHOD(&HeatmapLayer::getHeatmapColor, "nativeGetHeatmapColor"),
            METHOD(&HeatmapLayer::getHeatmapOpacityTransition, "nativeGetHeatmapOpacityTransition"),
            METHOD(&HeatmapLayer::setHeatmapOpacityTransition, "nativeSetHeatmapOpacityTransition"),
            METHOD(&HeatmapLayer::getHeatmapOpacity, "nativeGetHeatmapOpacity"));
    }

} // namespace android
} // namespace mbgl
