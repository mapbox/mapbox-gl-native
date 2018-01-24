// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "hillshade_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

namespace mbgl {
namespace android {

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    HillshadeLayer::HillshadeLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::HillshadeLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
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

    jni::Object<jni::ObjectTag> HillshadeLayer::getHillshadeIlluminationDirection(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::getHillshadeIlluminationDirection());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> HillshadeLayer::getHillshadeIlluminationAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::getHillshadeIlluminationAnchor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> HillshadeLayer::getHillshadeExaggeration(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::getHillshadeExaggeration());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> HillshadeLayer::getHillshadeExaggerationTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::getHillshadeExaggerationTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void HillshadeLayer::setHillshadeExaggerationTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::setHillshadeExaggerationTransition(options);
    }

    jni::Object<jni::ObjectTag> HillshadeLayer::getHillshadeShadowColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::getHillshadeShadowColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> HillshadeLayer::getHillshadeShadowColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::getHillshadeShadowColorTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void HillshadeLayer::setHillshadeShadowColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::setHillshadeShadowColorTransition(options);
    }

    jni::Object<jni::ObjectTag> HillshadeLayer::getHillshadeHighlightColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::getHillshadeHighlightColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> HillshadeLayer::getHillshadeHighlightColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::getHillshadeHighlightColorTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void HillshadeLayer::setHillshadeHighlightColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::setHillshadeHighlightColorTransition(options);
    }

    jni::Object<jni::ObjectTag> HillshadeLayer::getHillshadeAccentColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::getHillshadeAccentColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> HillshadeLayer::getHillshadeAccentColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::getHillshadeAccentColorTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void HillshadeLayer::setHillshadeAccentColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::HillshadeLayer>()->HillshadeLayer::setHillshadeAccentColorTransition(options);
    }


    jni::Class<HillshadeLayer> HillshadeLayer::javaClass;

    jni::jobject* HillshadeLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = HillshadeLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return HillshadeLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void HillshadeLayer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        HillshadeLayer::javaClass = *jni::Class<HillshadeLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<HillshadeLayer>(
            env, HillshadeLayer::javaClass, "nativePtr",
            std::make_unique<HillshadeLayer, JNIEnv&, jni::String, jni::String>,
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
