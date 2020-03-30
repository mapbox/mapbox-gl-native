// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "location_component_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace android {

    inline mbgl::style::LocationComponentLayer& toLocationComponentLayer(mbgl::style::Layer& layer) {
        return static_cast<mbgl::style::LocationComponentLayer&>(layer);
    }

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    LocationComponentLayer::LocationComponentLayer(jni::JNIEnv& env, jni::String& layerId)
        : Layer(std::make_unique<mbgl::style::LocationComponentLayer>(jni::Make<std::string>(env, layerId))) {
    }

    /**
     * Creates a non-owning peer object (for layers currently attached to the map)
     */
    LocationComponentLayer::LocationComponentLayer(mbgl::style::LocationComponentLayer& coreLayer)
        : Layer(coreLayer) {
    }

    /**
     * Creates an owning peer object (for layers not attached to the map)
     */
    LocationComponentLayer::LocationComponentLayer(std::unique_ptr<mbgl::style::LocationComponentLayer> coreLayer)
        : Layer(std::move(coreLayer)) {
    }

    LocationComponentLayer::~LocationComponentLayer() = default;

    // Property getters

    jni::Local<jni::Object<>> LocationComponentLayer::getTopImage(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getTopImage()));
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getBearingImage(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getBearingImage()));
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getShadowImage(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getShadowImage()));
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getLocation(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getLocation()));
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getBearing(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getBearing()));
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getAccuracyRadius(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getAccuracyRadius()));
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getTopImageSize(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getTopImageSize()));
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getBearingImageSize(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getBearingImageSize()));
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getShadowImageSize(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getShadowImageSize()));
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getPerspectiveCompensation(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getPerspectiveCompensation()));
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getImageTiltDisplacement(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getImageTiltDisplacement()));
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getAccuracyRadiusColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getAccuracyRadiusColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> LocationComponentLayer::getAccuracyRadiusColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toLocationComponentLayer(layer).getAccuracyRadiusColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void LocationComponentLayer::setAccuracyRadiusColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toLocationComponentLayer(layer).setAccuracyRadiusColorTransition(options);
    }

    jni::Local<jni::Object<>> LocationComponentLayer::getAccuracyRadiusBorderColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toLocationComponentLayer(layer).getAccuracyRadiusBorderColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> LocationComponentLayer::getAccuracyRadiusBorderColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toLocationComponentLayer(layer).getAccuracyRadiusBorderColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void LocationComponentLayer::setAccuracyRadiusBorderColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toLocationComponentLayer(layer).setAccuracyRadiusBorderColorTransition(options);
    }


    // LocationComponentJavaLayerPeerFactory

    LocationComponentJavaLayerPeerFactory::~LocationComponentJavaLayerPeerFactory() = default;

    namespace {
        jni::Local<jni::Object<Layer>> createJavaPeer(jni::JNIEnv& env, Layer* layer) {
            static auto& javaClass = jni::Class<LocationComponentLayer>::Singleton(env);
            static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
            return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(layer));
        }
    }  // namespace

    jni::Local<jni::Object<Layer>> LocationComponentJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, mbgl::style::Layer& layer) {
        assert(layer.baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new LocationComponentLayer(toLocationComponentLayer(layer)));
    }

    jni::Local<jni::Object<Layer>> LocationComponentJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, std::unique_ptr<mbgl::style::Layer> layer) {
        assert(layer->baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new LocationComponentLayer(std::unique_ptr<mbgl::style::LocationComponentLayer>(static_cast<mbgl::style::LocationComponentLayer*>(layer.release()))));
    }

    void LocationComponentJavaLayerPeerFactory::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto& javaClass = jni::Class<LocationComponentLayer>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<LocationComponentLayer>(
            env,
            javaClass,
            "nativePtr",
            jni::MakePeer<LocationComponentLayer, jni::String&>,
            "initialize",
            "finalize",
            METHOD(&LocationComponentLayer::getTopImage, "nativeGetTopImage"),
            METHOD(&LocationComponentLayer::getBearingImage, "nativeGetBearingImage"),
            METHOD(&LocationComponentLayer::getShadowImage, "nativeGetShadowImage"),
            METHOD(&LocationComponentLayer::getLocation, "nativeGetLocation"),
            METHOD(&LocationComponentLayer::getBearing, "nativeGetBearing"),
            METHOD(&LocationComponentLayer::getAccuracyRadius, "nativeGetAccuracyRadius"),
            METHOD(&LocationComponentLayer::getTopImageSize, "nativeGetTopImageSize"),
            METHOD(&LocationComponentLayer::getBearingImageSize, "nativeGetBearingImageSize"),
            METHOD(&LocationComponentLayer::getShadowImageSize, "nativeGetShadowImageSize"),
            METHOD(&LocationComponentLayer::getPerspectiveCompensation, "nativeGetPerspectiveCompensation"),
            METHOD(&LocationComponentLayer::getImageTiltDisplacement, "nativeGetImageTiltDisplacement"),
            METHOD(&LocationComponentLayer::getAccuracyRadiusColorTransition, "nativeGetAccuracyRadiusColorTransition"),
            METHOD(&LocationComponentLayer::setAccuracyRadiusColorTransition, "nativeSetAccuracyRadiusColorTransition"),
            METHOD(&LocationComponentLayer::getAccuracyRadiusColor, "nativeGetAccuracyRadiusColor"),
            METHOD(&LocationComponentLayer::getAccuracyRadiusBorderColorTransition, "nativeGetAccuracyRadiusBorderColorTransition"),
            METHOD(&LocationComponentLayer::setAccuracyRadiusBorderColorTransition, "nativeSetAccuracyRadiusBorderColorTransition"),
            METHOD(&LocationComponentLayer::getAccuracyRadiusBorderColor, "nativeGetAccuracyRadiusBorderColor"));
    }

} // namespace android
} // namespace mbgl
