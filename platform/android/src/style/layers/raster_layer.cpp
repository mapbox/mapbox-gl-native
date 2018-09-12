// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "raster_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

namespace mbgl {
namespace android {

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    RasterLayer::RasterLayer(jni::JNIEnv& env, jni::String& layerId, jni::String& sourceId)
        : Layer(env, std::make_unique<mbgl::style::RasterLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
    }

    /**
     * Creates a non-owning peer object (for layers currently attached to the map)
     */
    RasterLayer::RasterLayer(mbgl::Map& map, mbgl::style::RasterLayer& coreLayer)
        : Layer(map, coreLayer) {
    }

    /**
     * Creates an owning peer object (for layers not attached to the map)
     */
    RasterLayer::RasterLayer(mbgl::Map& map, std::unique_ptr<mbgl::style::RasterLayer> coreLayer)
        : Layer(map, std::move(coreLayer)) {
    }

    RasterLayer::~RasterLayer() = default;

    // Property getters

    jni::Local<jni::Object<>> RasterLayer::getRasterOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterOpacity()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterOpacityTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::RasterLayer>()->RasterLayer::setRasterOpacityTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterHueRotate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterHueRotate()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterHueRotateTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterHueRotateTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterHueRotateTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::RasterLayer>()->RasterLayer::setRasterHueRotateTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterBrightnessMin(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterBrightnessMin()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterBrightnessMinTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterBrightnessMinTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterBrightnessMinTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::RasterLayer>()->RasterLayer::setRasterBrightnessMinTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterBrightnessMax(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterBrightnessMax()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterBrightnessMaxTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterBrightnessMaxTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterBrightnessMaxTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::RasterLayer>()->RasterLayer::setRasterBrightnessMaxTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterSaturation(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterSaturation()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterSaturationTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterSaturationTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterSaturationTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::RasterLayer>()->RasterLayer::setRasterSaturationTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterContrast(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterContrast()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterContrastTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterContrastTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterContrastTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::RasterLayer>()->RasterLayer::setRasterContrastTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterResampling(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterResampling()));
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterFadeDuration(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::RasterLayer>()->RasterLayer::getRasterFadeDuration()));
    }


    jni::Local<jni::Object<Layer>> RasterLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto& javaClass = jni::Class<RasterLayer>::Singleton(env);
        static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
        return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void RasterLayer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto& javaClass = jni::Class<RasterLayer>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<RasterLayer>(
            env, javaClass, "nativePtr",
            jni::MakePeer<RasterLayer, jni::String&, jni::String&>,
            "initialize",
            "finalize",
            METHOD(&RasterLayer::getRasterOpacityTransition, "nativeGetRasterOpacityTransition"),
            METHOD(&RasterLayer::setRasterOpacityTransition, "nativeSetRasterOpacityTransition"),
            METHOD(&RasterLayer::getRasterOpacity, "nativeGetRasterOpacity"),
            METHOD(&RasterLayer::getRasterHueRotateTransition, "nativeGetRasterHueRotateTransition"),
            METHOD(&RasterLayer::setRasterHueRotateTransition, "nativeSetRasterHueRotateTransition"),
            METHOD(&RasterLayer::getRasterHueRotate, "nativeGetRasterHueRotate"),
            METHOD(&RasterLayer::getRasterBrightnessMinTransition, "nativeGetRasterBrightnessMinTransition"),
            METHOD(&RasterLayer::setRasterBrightnessMinTransition, "nativeSetRasterBrightnessMinTransition"),
            METHOD(&RasterLayer::getRasterBrightnessMin, "nativeGetRasterBrightnessMin"),
            METHOD(&RasterLayer::getRasterBrightnessMaxTransition, "nativeGetRasterBrightnessMaxTransition"),
            METHOD(&RasterLayer::setRasterBrightnessMaxTransition, "nativeSetRasterBrightnessMaxTransition"),
            METHOD(&RasterLayer::getRasterBrightnessMax, "nativeGetRasterBrightnessMax"),
            METHOD(&RasterLayer::getRasterSaturationTransition, "nativeGetRasterSaturationTransition"),
            METHOD(&RasterLayer::setRasterSaturationTransition, "nativeSetRasterSaturationTransition"),
            METHOD(&RasterLayer::getRasterSaturation, "nativeGetRasterSaturation"),
            METHOD(&RasterLayer::getRasterContrastTransition, "nativeGetRasterContrastTransition"),
            METHOD(&RasterLayer::setRasterContrastTransition, "nativeSetRasterContrastTransition"),
            METHOD(&RasterLayer::getRasterContrast, "nativeGetRasterContrast"),
            METHOD(&RasterLayer::getRasterResampling, "nativeGetRasterResampling"),
            METHOD(&RasterLayer::getRasterFadeDuration, "nativeGetRasterFadeDuration"));
    }

} // namespace android
} // namespace mbgl
