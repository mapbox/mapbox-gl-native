// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "fill_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

namespace mbgl {
namespace android {

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    FillLayer::FillLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::FillLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
    }

    /**
     * Creates a non-owning peer object (for layers currently attached to the map)
     */
    FillLayer::FillLayer(mbgl::Map& map, mbgl::style::FillLayer& coreLayer)
        : Layer(map, coreLayer) {
    }

    /**
     * Creates an owning peer object (for layers not attached to the map)
     */
    FillLayer::FillLayer(mbgl::Map& map, std::unique_ptr<mbgl::style::FillLayer> coreLayer)
        : Layer(map, std::move(coreLayer)) {
    }

    FillLayer::~FillLayer() = default;

    // Property getters

    jni::Object<jni::ObjectTag> FillLayer::getFillAntialias(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillLayer>()->FillLayer::getFillAntialias());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> FillLayer::getFillOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillLayer>()->FillLayer::getFillOpacity());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> FillLayer::getFillOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::FillLayer>()->FillLayer::getFillOpacityTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void FillLayer::setFillOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::FillLayer>()->FillLayer::setFillOpacityTransition(options);
    }

    jni::Object<jni::ObjectTag> FillLayer::getFillColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillLayer>()->FillLayer::getFillColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> FillLayer::getFillColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::FillLayer>()->FillLayer::getFillColorTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void FillLayer::setFillColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::FillLayer>()->FillLayer::setFillColorTransition(options);
    }

    jni::Object<jni::ObjectTag> FillLayer::getFillOutlineColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillLayer>()->FillLayer::getFillOutlineColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> FillLayer::getFillOutlineColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::FillLayer>()->FillLayer::getFillOutlineColorTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void FillLayer::setFillOutlineColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::FillLayer>()->FillLayer::setFillOutlineColorTransition(options);
    }

    jni::Object<jni::ObjectTag> FillLayer::getFillTranslate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillLayer>()->FillLayer::getFillTranslate());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> FillLayer::getFillTranslateTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::FillLayer>()->FillLayer::getFillTranslateTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void FillLayer::setFillTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::FillLayer>()->FillLayer::setFillTranslateTransition(options);
    }

    jni::Object<jni::ObjectTag> FillLayer::getFillTranslateAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillLayer>()->FillLayer::getFillTranslateAnchor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> FillLayer::getFillPattern(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillLayer>()->FillLayer::getFillPattern());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<TransitionOptions> FillLayer::getFillPatternTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::FillLayer>()->FillLayer::getFillPatternTransition();
        return *convert<jni::Object<TransitionOptions>>(env, options);
    }

    void FillLayer::setFillPatternTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::FillLayer>()->FillLayer::setFillPatternTransition(options);
    }


    jni::Class<FillLayer> FillLayer::javaClass;

    jni::jobject* FillLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = FillLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return FillLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void FillLayer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        FillLayer::javaClass = *jni::Class<FillLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<FillLayer>(
            env, FillLayer::javaClass, "nativePtr",
            std::make_unique<FillLayer, JNIEnv&, jni::String, jni::String>,
            "initialize",
            "finalize",
            METHOD(&FillLayer::getFillAntialias, "nativeGetFillAntialias"),
            METHOD(&FillLayer::getFillOpacityTransition, "nativeGetFillOpacityTransition"),
            METHOD(&FillLayer::setFillOpacityTransition, "nativeSetFillOpacityTransition"),
            METHOD(&FillLayer::getFillOpacity, "nativeGetFillOpacity"),
            METHOD(&FillLayer::getFillColorTransition, "nativeGetFillColorTransition"),
            METHOD(&FillLayer::setFillColorTransition, "nativeSetFillColorTransition"),
            METHOD(&FillLayer::getFillColor, "nativeGetFillColor"),
            METHOD(&FillLayer::getFillOutlineColorTransition, "nativeGetFillOutlineColorTransition"),
            METHOD(&FillLayer::setFillOutlineColorTransition, "nativeSetFillOutlineColorTransition"),
            METHOD(&FillLayer::getFillOutlineColor, "nativeGetFillOutlineColor"),
            METHOD(&FillLayer::getFillTranslateTransition, "nativeGetFillTranslateTransition"),
            METHOD(&FillLayer::setFillTranslateTransition, "nativeSetFillTranslateTransition"),
            METHOD(&FillLayer::getFillTranslate, "nativeGetFillTranslate"),
            METHOD(&FillLayer::getFillTranslateAnchor, "nativeGetFillTranslateAnchor"),
            METHOD(&FillLayer::getFillPatternTransition, "nativeGetFillPatternTransition"),
            METHOD(&FillLayer::setFillPatternTransition, "nativeSetFillPatternTransition"),
            METHOD(&FillLayer::getFillPattern, "nativeGetFillPattern"));
    }

} // namespace android
} // namespace mbgl
