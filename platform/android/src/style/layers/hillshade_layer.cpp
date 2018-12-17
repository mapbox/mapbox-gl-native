// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "hillshade_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace android {

    inline mbgl::style::HillshadeLayer& toHillshadeLayer(mbgl::style::Layer& layer) {
        return static_cast<mbgl::style::HillshadeLayer&>(layer);
    }

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    HillshadeLayer::HillshadeLayer(jni::JNIEnv& env, jni::String& layerId, jni::String& sourceId)
        : Layer(std::make_unique<mbgl::style::HillshadeLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
    }

    /**
     * Creates a non-owning peer object (for layers currently attached to the map)
     */
    HillshadeLayer::HillshadeLayer(mbgl::Map& map, mbgl::style::HillshadeLayer& coreLayer)
        : Layer(map, coreLayer) {
    }

    /**
     * Creates an owning peer object (for layers not attached to the map)
     */
    HillshadeLayer::HillshadeLayer(mbgl::Map& map, std::unique_ptr<mbgl::style::HillshadeLayer> coreLayer)
        : Layer(map, std::move(coreLayer)) {
    }

    HillshadeLayer::~HillshadeLayer() = default;

    // Property getters

    jni::Local<jni::Object<>> HillshadeLayer::getHillshadeIlluminationDirection(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toHillshadeLayer(layer).getHillshadeIlluminationDirection()));
    }

    jni::Local<jni::Object<>> HillshadeLayer::getHillshadeIlluminationAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toHillshadeLayer(layer).getHillshadeIlluminationAnchor()));
    }

    jni::Local<jni::Object<>> HillshadeLayer::getHillshadeExaggeration(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toHillshadeLayer(layer).getHillshadeExaggeration()));
    }

    jni::Local<jni::Object<TransitionOptions>> HillshadeLayer::getHillshadeExaggerationTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toHillshadeLayer(layer).getHillshadeExaggerationTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void HillshadeLayer::setHillshadeExaggerationTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toHillshadeLayer(layer).setHillshadeExaggerationTransition(options);
    }

    jni::Local<jni::Object<>> HillshadeLayer::getHillshadeShadowColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toHillshadeLayer(layer).getHillshadeShadowColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> HillshadeLayer::getHillshadeShadowColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toHillshadeLayer(layer).getHillshadeShadowColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void HillshadeLayer::setHillshadeShadowColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toHillshadeLayer(layer).setHillshadeShadowColorTransition(options);
    }

    jni::Local<jni::Object<>> HillshadeLayer::getHillshadeHighlightColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toHillshadeLayer(layer).getHillshadeHighlightColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> HillshadeLayer::getHillshadeHighlightColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toHillshadeLayer(layer).getHillshadeHighlightColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void HillshadeLayer::setHillshadeHighlightColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toHillshadeLayer(layer).setHillshadeHighlightColorTransition(options);
    }

    jni::Local<jni::Object<>> HillshadeLayer::getHillshadeAccentColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toHillshadeLayer(layer).getHillshadeAccentColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> HillshadeLayer::getHillshadeAccentColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toHillshadeLayer(layer).getHillshadeAccentColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void HillshadeLayer::setHillshadeAccentColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toHillshadeLayer(layer).setHillshadeAccentColorTransition(options);
    }


    // HillshadeJavaLayerPeerFactory

    HillshadeJavaLayerPeerFactory::~HillshadeJavaLayerPeerFactory() = default;

    namespace {
        jni::Local<jni::Object<Layer>> createJavaPeer(jni::JNIEnv& env, Layer* layer) {
            static auto& javaClass = jni::Class<HillshadeLayer>::Singleton(env);
            static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
            return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(layer));
        }
    }  // namespace

    jni::Local<jni::Object<Layer>> HillshadeJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, mbgl::style::Layer& layer) {
        assert(layer.baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new HillshadeLayer(map, toHillshadeLayer(layer)));
    }

    jni::Local<jni::Object<Layer>> HillshadeJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer> layer) {
        assert(layer->baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new HillshadeLayer(map, std::unique_ptr<mbgl::style::HillshadeLayer>(static_cast<mbgl::style::HillshadeLayer*>(layer.release()))));
    }

    void HillshadeJavaLayerPeerFactory::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto& javaClass = jni::Class<HillshadeLayer>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<HillshadeLayer>(
            env, javaClass, "nativePtr",
            jni::MakePeer<HillshadeLayer, jni::String&, jni::String&>,
            "initialize",
            "finalize",
            METHOD(&HillshadeLayer::getHillshadeIlluminationDirection, "nativeGetHillshadeIlluminationDirection"),
            METHOD(&HillshadeLayer::getHillshadeIlluminationAnchor, "nativeGetHillshadeIlluminationAnchor"),
            METHOD(&HillshadeLayer::getHillshadeExaggerationTransition, "nativeGetHillshadeExaggerationTransition"),
            METHOD(&HillshadeLayer::setHillshadeExaggerationTransition, "nativeSetHillshadeExaggerationTransition"),
            METHOD(&HillshadeLayer::getHillshadeExaggeration, "nativeGetHillshadeExaggeration"),
            METHOD(&HillshadeLayer::getHillshadeShadowColorTransition, "nativeGetHillshadeShadowColorTransition"),
            METHOD(&HillshadeLayer::setHillshadeShadowColorTransition, "nativeSetHillshadeShadowColorTransition"),
            METHOD(&HillshadeLayer::getHillshadeShadowColor, "nativeGetHillshadeShadowColor"),
            METHOD(&HillshadeLayer::getHillshadeHighlightColorTransition, "nativeGetHillshadeHighlightColorTransition"),
            METHOD(&HillshadeLayer::setHillshadeHighlightColorTransition, "nativeSetHillshadeHighlightColorTransition"),
            METHOD(&HillshadeLayer::getHillshadeHighlightColor, "nativeGetHillshadeHighlightColor"),
            METHOD(&HillshadeLayer::getHillshadeAccentColorTransition, "nativeGetHillshadeAccentColorTransition"),
            METHOD(&HillshadeLayer::setHillshadeAccentColorTransition, "nativeSetHillshadeAccentColorTransition"),
            METHOD(&HillshadeLayer::getHillshadeAccentColor, "nativeGetHillshadeAccentColor"));
    }

} // namespace android
} // namespace mbgl
