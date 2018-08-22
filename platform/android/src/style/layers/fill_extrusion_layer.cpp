// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "fill_extrusion_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

namespace mbgl {
namespace android {

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    FillExtrusionLayer::FillExtrusionLayer(jni::JNIEnv& env, jni::String& layerId, jni::String& sourceId)
        : Layer(env, std::make_unique<mbgl::style::FillExtrusionLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
    }

    /**
     * Creates a non-owning peer object (for layers currently attached to the map)
     */
    FillExtrusionLayer::FillExtrusionLayer(mbgl::Map& map, mbgl::style::FillExtrusionLayer& coreLayer)
        : Layer(map, coreLayer) {
    }

    /**
     * Creates an owning peer object (for layers not attached to the map)
     */
    FillExtrusionLayer::FillExtrusionLayer(mbgl::Map& map, std::unique_ptr<mbgl::style::FillExtrusionLayer> coreLayer)
        : Layer(map, std::move(coreLayer)) {
    }

    FillExtrusionLayer::~FillExtrusionLayer() = default;

    // Property getters

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionOpacity()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionOpacityTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::setFillExtrusionOpacityTransition(options);
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::setFillExtrusionColorTransition(options);
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionTranslate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionTranslate()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionTranslateTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionTranslateTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::setFillExtrusionTranslateTransition(options);
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionTranslateAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionTranslateAnchor()));
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionPattern(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionPattern()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionPatternTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionPatternTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionPatternTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::setFillExtrusionPatternTransition(options);
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionHeight(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionHeight()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionHeightTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionHeightTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionHeightTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::setFillExtrusionHeightTransition(options);
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionBase(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionBase()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionBaseTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionBaseTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionBaseTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::setFillExtrusionBaseTransition(options);
    }


    jni::Local<jni::Object<Layer>> FillExtrusionLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto& javaClass = jni::Class<FillExtrusionLayer>::Singleton(env);
        static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
        return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void FillExtrusionLayer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto& javaClass = jni::Class<FillExtrusionLayer>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<FillExtrusionLayer>(
            env, javaClass, "nativePtr",
            jni::MakePeer<FillExtrusionLayer, jni::String&, jni::String&>,
            "initialize",
            "finalize",
            METHOD(&FillExtrusionLayer::getFillExtrusionOpacityTransition, "nativeGetFillExtrusionOpacityTransition"),
            METHOD(&FillExtrusionLayer::setFillExtrusionOpacityTransition, "nativeSetFillExtrusionOpacityTransition"),
            METHOD(&FillExtrusionLayer::getFillExtrusionOpacity, "nativeGetFillExtrusionOpacity"),
            METHOD(&FillExtrusionLayer::getFillExtrusionColorTransition, "nativeGetFillExtrusionColorTransition"),
            METHOD(&FillExtrusionLayer::setFillExtrusionColorTransition, "nativeSetFillExtrusionColorTransition"),
            METHOD(&FillExtrusionLayer::getFillExtrusionColor, "nativeGetFillExtrusionColor"),
            METHOD(&FillExtrusionLayer::getFillExtrusionTranslateTransition, "nativeGetFillExtrusionTranslateTransition"),
            METHOD(&FillExtrusionLayer::setFillExtrusionTranslateTransition, "nativeSetFillExtrusionTranslateTransition"),
            METHOD(&FillExtrusionLayer::getFillExtrusionTranslate, "nativeGetFillExtrusionTranslate"),
            METHOD(&FillExtrusionLayer::getFillExtrusionTranslateAnchor, "nativeGetFillExtrusionTranslateAnchor"),
            METHOD(&FillExtrusionLayer::getFillExtrusionPatternTransition, "nativeGetFillExtrusionPatternTransition"),
            METHOD(&FillExtrusionLayer::setFillExtrusionPatternTransition, "nativeSetFillExtrusionPatternTransition"),
            METHOD(&FillExtrusionLayer::getFillExtrusionPattern, "nativeGetFillExtrusionPattern"),
            METHOD(&FillExtrusionLayer::getFillExtrusionHeightTransition, "nativeGetFillExtrusionHeightTransition"),
            METHOD(&FillExtrusionLayer::setFillExtrusionHeightTransition, "nativeSetFillExtrusionHeightTransition"),
            METHOD(&FillExtrusionLayer::getFillExtrusionHeight, "nativeGetFillExtrusionHeight"),
            METHOD(&FillExtrusionLayer::getFillExtrusionBaseTransition, "nativeGetFillExtrusionBaseTransition"),
            METHOD(&FillExtrusionLayer::setFillExtrusionBaseTransition, "nativeSetFillExtrusionBaseTransition"),
            METHOD(&FillExtrusionLayer::getFillExtrusionBase, "nativeGetFillExtrusionBase"));
    }

} // namespace android
} // namespace mbgl
