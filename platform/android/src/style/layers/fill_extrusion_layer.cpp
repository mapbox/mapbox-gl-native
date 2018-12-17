// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "fill_extrusion_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"
#include "../conversion/transition_options.hpp"

#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace android {

    inline mbgl::style::FillExtrusionLayer& toFillExtrusionLayer(mbgl::style::Layer& layer) {
        return static_cast<mbgl::style::FillExtrusionLayer&>(layer);
    }

    /**
     * Creates an owning peer object (for layers not attached to the map) from the JVM side
     */
    FillExtrusionLayer::FillExtrusionLayer(jni::JNIEnv& env, jni::String& layerId, jni::String& sourceId)
        : Layer(std::make_unique<mbgl::style::FillExtrusionLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
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
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toFillExtrusionLayer(layer).getFillExtrusionOpacity()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionOpacityTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toFillExtrusionLayer(layer).getFillExtrusionOpacityTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toFillExtrusionLayer(layer).setFillExtrusionOpacityTransition(options);
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toFillExtrusionLayer(layer).getFillExtrusionColor()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionColorTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toFillExtrusionLayer(layer).getFillExtrusionColorTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toFillExtrusionLayer(layer).setFillExtrusionColorTransition(options);
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionTranslate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toFillExtrusionLayer(layer).getFillExtrusionTranslate()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionTranslateTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toFillExtrusionLayer(layer).getFillExtrusionTranslateTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionTranslateTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toFillExtrusionLayer(layer).setFillExtrusionTranslateTransition(options);
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionTranslateAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toFillExtrusionLayer(layer).getFillExtrusionTranslateAnchor()));
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionPattern(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toFillExtrusionLayer(layer).getFillExtrusionPattern()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionPatternTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toFillExtrusionLayer(layer).getFillExtrusionPatternTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionPatternTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toFillExtrusionLayer(layer).setFillExtrusionPatternTransition(options);
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionHeight(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toFillExtrusionLayer(layer).getFillExtrusionHeight()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionHeightTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toFillExtrusionLayer(layer).getFillExtrusionHeightTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionHeightTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toFillExtrusionLayer(layer).setFillExtrusionHeightTransition(options);
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionBase(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toFillExtrusionLayer(layer).getFillExtrusionBase()));
    }

    jni::Local<jni::Object<TransitionOptions>> FillExtrusionLayer::getFillExtrusionBaseTransition(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        mbgl::style::TransitionOptions options = toFillExtrusionLayer(layer).getFillExtrusionBaseTransition();
        return std::move(*convert<jni::Local<jni::Object<TransitionOptions>>>(env, options));
    }

    void FillExtrusionLayer::setFillExtrusionBaseTransition(jni::JNIEnv&, jlong duration, jlong delay) {
        mbgl::style::TransitionOptions options;
        options.duration.emplace(mbgl::Milliseconds(duration));
        options.delay.emplace(mbgl::Milliseconds(delay));
        toFillExtrusionLayer(layer).setFillExtrusionBaseTransition(options);
    }

    jni::Local<jni::Object<>> FillExtrusionLayer::getFillExtrusionVerticalGradient(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, toFillExtrusionLayer(layer).getFillExtrusionVerticalGradient()));
    }


    // FillExtrusionJavaLayerPeerFactory

    FillExtrusionJavaLayerPeerFactory::~FillExtrusionJavaLayerPeerFactory() = default;

    namespace {
        jni::Local<jni::Object<Layer>> createJavaPeer(jni::JNIEnv& env, Layer* layer) {
            static auto& javaClass = jni::Class<FillExtrusionLayer>::Singleton(env);
            static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
            return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(layer));
        }
    }  // namespace

    jni::Local<jni::Object<Layer>> FillExtrusionJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, mbgl::style::Layer& layer) {
        assert(layer.baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new FillExtrusionLayer(map, toFillExtrusionLayer(layer)));
    }

    jni::Local<jni::Object<Layer>> FillExtrusionJavaLayerPeerFactory::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer> layer) {
        assert(layer->baseImpl->getTypeInfo() == getTypeInfo());
        return createJavaPeer(env, new FillExtrusionLayer(map, std::unique_ptr<mbgl::style::FillExtrusionLayer>(static_cast<mbgl::style::FillExtrusionLayer*>(layer.release()))));
    }

    void FillExtrusionJavaLayerPeerFactory::registerNative(jni::JNIEnv& env) {
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
            METHOD(&FillExtrusionLayer::getFillExtrusionBase, "nativeGetFillExtrusionBase"),
            METHOD(&FillExtrusionLayer::getFillExtrusionVerticalGradient, "nativeGetFillExtrusionVerticalGradient"));
    }

} // namespace android
} // namespace mbgl
