// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "heatmap_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

namespace mbgl {
namespace android {

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    HeatmapLayer::HeatmapLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::HeatmapLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
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

    jni::Object<jni::ObjectTag> HeatmapLayer::getHeatmapRadius(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::HeatmapLayer>()->HeatmapLayer::getHeatmapRadius());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> HeatmapLayer::getHeatmapRadiusTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::HeatmapLayer>()->HeatmapLayer::getHeatmapRadiusTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void HeatmapLayer::setHeatmapRadiusTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::HeatmapLayer>()->HeatmapLayer::setHeatmapRadiusTransition(options);
    }

    jni::Object<jni::ObjectTag> HeatmapLayer::getHeatmapWeight(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::HeatmapLayer>()->HeatmapLayer::getHeatmapWeight());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> HeatmapLayer::getHeatmapIntensity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::HeatmapLayer>()->HeatmapLayer::getHeatmapIntensity());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> HeatmapLayer::getHeatmapIntensityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::HeatmapLayer>()->HeatmapLayer::getHeatmapIntensityTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void HeatmapLayer::setHeatmapIntensityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::HeatmapLayer>()->HeatmapLayer::setHeatmapIntensityTransition(options);
    }

    jni::Object<jni::ObjectTag> HeatmapLayer::getHeatmapColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::HeatmapLayer>()->HeatmapLayer::getHeatmapColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> HeatmapLayer::getHeatmapOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::HeatmapLayer>()->HeatmapLayer::getHeatmapOpacity());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> HeatmapLayer::getHeatmapOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::HeatmapLayer>()->HeatmapLayer::getHeatmapOpacityTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void HeatmapLayer::setHeatmapOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::HeatmapLayer>()->HeatmapLayer::setHeatmapOpacityTransition(options);
    }


    jni::Class<HeatmapLayer> HeatmapLayer::javaClass;

    jni::jobject* HeatmapLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = HeatmapLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return HeatmapLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void HeatmapLayer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        HeatmapLayer::javaClass = *jni::Class<HeatmapLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<HeatmapLayer>(
            env, HeatmapLayer::javaClass, "nativePtr",
            std::make_unique<HeatmapLayer, JNIEnv&, jni::String, jni::String>,
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
