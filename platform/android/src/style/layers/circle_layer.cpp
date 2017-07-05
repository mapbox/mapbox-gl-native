// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "circle_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

namespace mbgl {
namespace android {

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    CircleLayer::CircleLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::CircleLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
    }

    /**
     * Creates a non-owning peer object (for layers currently attached to the map)
     */
    CircleLayer::CircleLayer(mbgl::Map& map, mbgl::style::CircleLayer& coreLayer)
        : Layer(map, coreLayer) {
    }

    /**
     * Creates an owning peer object (for layers not attached to the map)
     */
    CircleLayer::CircleLayer(mbgl::Map& map, std::unique_ptr<mbgl::style::CircleLayer> coreLayer)
        : Layer(map, std::move(coreLayer)) {
    }

    CircleLayer::~CircleLayer() = default;

    // Property getters

    jni::Object<jni::ObjectTag> CircleLayer::getCircleRadius(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleRadius());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> CircleLayer::getCircleRadiusTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleRadiusTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void CircleLayer::setCircleRadiusTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::CircleLayer>()->CircleLayer::setCircleRadiusTransition(options);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> CircleLayer::getCircleColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleColorTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void CircleLayer::setCircleColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::CircleLayer>()->CircleLayer::setCircleColorTransition(options);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleBlur(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleBlur());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> CircleLayer::getCircleBlurTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleBlurTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void CircleLayer::setCircleBlurTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::CircleLayer>()->CircleLayer::setCircleBlurTransition(options);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleOpacity());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> CircleLayer::getCircleOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleOpacityTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void CircleLayer::setCircleOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::CircleLayer>()->CircleLayer::setCircleOpacityTransition(options);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleTranslate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleTranslate());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> CircleLayer::getCircleTranslateTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleTranslateTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void CircleLayer::setCircleTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::CircleLayer>()->CircleLayer::setCircleTranslateTransition(options);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleTranslateAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleTranslateAnchor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCirclePitchScale(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCirclePitchScale());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCirclePitchAlignment(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCirclePitchAlignment());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleStrokeWidth(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleStrokeWidth());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> CircleLayer::getCircleStrokeWidthTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleStrokeWidthTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void CircleLayer::setCircleStrokeWidthTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::CircleLayer>()->CircleLayer::setCircleStrokeWidthTransition(options);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleStrokeColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleStrokeColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> CircleLayer::getCircleStrokeColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleStrokeColorTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void CircleLayer::setCircleStrokeColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::CircleLayer>()->CircleLayer::setCircleStrokeColorTransition(options);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleStrokeOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleStrokeOpacity());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> CircleLayer::getCircleStrokeOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleStrokeOpacityTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void CircleLayer::setCircleStrokeOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::CircleLayer>()->CircleLayer::setCircleStrokeOpacityTransition(options);
    }


    jni::Class<CircleLayer> CircleLayer::javaClass;

    jni::jobject* CircleLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = CircleLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return CircleLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void CircleLayer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        CircleLayer::javaClass = *jni::Class<CircleLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<CircleLayer>(
            env, CircleLayer::javaClass, "nativePtr",
            std::make_unique<CircleLayer, JNIEnv&, jni::String, jni::String>,
            "initialize",
            "finalize",
            METHOD(&CircleLayer::getCircleRadiusTransition, "nativeGetCircleRadiusTransition"),
            METHOD(&CircleLayer::setCircleRadiusTransition, "nativeSetCircleRadiusTransition"),
            METHOD(&CircleLayer::getCircleRadius, "nativeGetCircleRadius"),
            METHOD(&CircleLayer::getCircleColorTransition, "nativeGetCircleColorTransition"),
            METHOD(&CircleLayer::setCircleColorTransition, "nativeSetCircleColorTransition"),
            METHOD(&CircleLayer::getCircleColor, "nativeGetCircleColor"),
            METHOD(&CircleLayer::getCircleBlurTransition, "nativeGetCircleBlurTransition"),
            METHOD(&CircleLayer::setCircleBlurTransition, "nativeSetCircleBlurTransition"),
            METHOD(&CircleLayer::getCircleBlur, "nativeGetCircleBlur"),
            METHOD(&CircleLayer::getCircleOpacityTransition, "nativeGetCircleOpacityTransition"),
            METHOD(&CircleLayer::setCircleOpacityTransition, "nativeSetCircleOpacityTransition"),
            METHOD(&CircleLayer::getCircleOpacity, "nativeGetCircleOpacity"),
            METHOD(&CircleLayer::getCircleTranslateTransition, "nativeGetCircleTranslateTransition"),
            METHOD(&CircleLayer::setCircleTranslateTransition, "nativeSetCircleTranslateTransition"),
            METHOD(&CircleLayer::getCircleTranslate, "nativeGetCircleTranslate"),
            METHOD(&CircleLayer::getCircleTranslateAnchor, "nativeGetCircleTranslateAnchor"),
            METHOD(&CircleLayer::getCirclePitchScale, "nativeGetCirclePitchScale"),
            METHOD(&CircleLayer::getCirclePitchAlignment, "nativeGetCirclePitchAlignment"),
            METHOD(&CircleLayer::getCircleStrokeWidthTransition, "nativeGetCircleStrokeWidthTransition"),
            METHOD(&CircleLayer::setCircleStrokeWidthTransition, "nativeSetCircleStrokeWidthTransition"),
            METHOD(&CircleLayer::getCircleStrokeWidth, "nativeGetCircleStrokeWidth"),
            METHOD(&CircleLayer::getCircleStrokeColorTransition, "nativeGetCircleStrokeColorTransition"),
            METHOD(&CircleLayer::setCircleStrokeColorTransition, "nativeSetCircleStrokeColorTransition"),
            METHOD(&CircleLayer::getCircleStrokeColor, "nativeGetCircleStrokeColor"),
            METHOD(&CircleLayer::getCircleStrokeOpacityTransition, "nativeGetCircleStrokeOpacityTransition"),
            METHOD(&CircleLayer::setCircleStrokeOpacityTransition, "nativeSetCircleStrokeOpacityTransition"),
            METHOD(&CircleLayer::getCircleStrokeOpacity, "nativeGetCircleStrokeOpacity"));
    }

} // namespace android
} // namespace mbgl
