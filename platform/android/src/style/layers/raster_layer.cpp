// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "raster_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace android {

    inline mbgl::style::RasterLayer& toRasterLayer(mbgl::style::Layer& layer) {
        return static_cast<mbgl::style::RasterLayer&>(layer);
    }

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    RasterLayer::RasterLayer(jni::JNIEnv& env, jni::String& layerId, jni::String& sourceId)
        : Layer(std::make_unique<mbgl::style::RasterLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
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
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toRasterLayer(layer).getRasterOpacity()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toRasterLayer(layer).getRasterOpacityTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toRasterLayer(layer).setRasterOpacityTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterHueRotate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toRasterLayer(layer).getRasterHueRotate()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterHueRotateTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toRasterLayer(layer).getRasterHueRotateTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterHueRotateTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toRasterLayer(layer).setRasterHueRotateTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterBrightnessMin(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toRasterLayer(layer).getRasterBrightnessMin()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterBrightnessMinTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toRasterLayer(layer).getRasterBrightnessMinTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterBrightnessMinTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toRasterLayer(layer).setRasterBrightnessMinTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterBrightnessMax(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toRasterLayer(layer).getRasterBrightnessMax()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterBrightnessMaxTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toRasterLayer(layer).getRasterBrightnessMaxTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterBrightnessMaxTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toRasterLayer(layer).setRasterBrightnessMaxTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterSaturation(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toRasterLayer(layer).getRasterSaturation()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterSaturationTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toRasterLayer(layer).getRasterSaturationTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterSaturationTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toRasterLayer(layer).setRasterSaturationTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterContrast(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toRasterLayer(layer).getRasterContrast()));
    }

    jni::Local<jni::Object<TransitionOptions>> RasterLayer::getRasterContrastTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toRasterLayer(layer).getRasterContrastTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void RasterLayer::setRasterContrastTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toRasterLayer(layer).setRasterContrastTransition(options);
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterResampling(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toRasterLayer(layer).getRasterResampling()));
    }

    jni::Local<jni::Object<>> RasterLayer::getRasterFadeDuration(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toRasterLayer(layer).getRasterFadeDuration()));
    }


    // RasterJavaLayerPeerFactory

    RasterJavaLayerPeerFactory::~RasterJavaLayerPeerFactory() = default;

    namespace {
        jni::Local<jni::Object<Layer>> createJavaPeer(jni::JNIEnv& env, Layer* layer) {
            static auto& javaClass = jni::Class<RasterLayer>::Singleton(env);
            static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
            return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(layer));
        }
    }  // namespace

    jni::Local<jni::Object<Layer>> RasterJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, mbgl::style::Layer& layer) {
        assert(layer.baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new RasterLayer(map, toRasterLayer(layer)));
    }

    jni::Local<jni::Object<Layer>> RasterJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer> layer) {
        assert(layer->baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new RasterLayer(map, std::unique_ptr<mbgl::style::RasterLayer>(static_cast<mbgl::style::RasterLayer*>(layer.release()))));
    }

    void RasterJavaLayerPeerFactory::registerNative(jni::JNIEnv& env) {
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
