// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "fill_extrusion_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"

namespace mbgl {
namespace android {

    FillExtrusionLayer::FillExtrusionLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::FillExtrusionLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
    }

    FillExtrusionLayer::FillExtrusionLayer(mbgl::Map& map, mbgl::style::FillExtrusionLayer& coreLayer)
        : Layer(map, coreLayer) {
    }

    FillExtrusionLayer::~FillExtrusionLayer() = default;

    // Property getters

    jni::Object<jni::ObjectTag> FillExtrusionLayer::getFillExtrusionOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionOpacity());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> FillExtrusionLayer::getFillExtrusionColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> FillExtrusionLayer::getFillExtrusionTranslate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionTranslate());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> FillExtrusionLayer::getFillExtrusionTranslateAnchor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionTranslateAnchor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> FillExtrusionLayer::getFillExtrusionPattern(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionPattern());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> FillExtrusionLayer::getFillExtrusionHeight(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionHeight());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> FillExtrusionLayer::getFillExtrusionBase(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillExtrusionLayer>()->FillExtrusionLayer::getFillExtrusionBase());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Class<FillExtrusionLayer> FillExtrusionLayer::javaClass;

    jni::jobject* FillExtrusionLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = FillExtrusionLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return FillExtrusionLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void FillExtrusionLayer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        FillExtrusionLayer::javaClass = *jni::Class<FillExtrusionLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<FillExtrusionLayer>(
            env, FillExtrusionLayer::javaClass, "nativePtr",
            std::make_unique<FillExtrusionLayer, JNIEnv&, jni::String, jni::String>,
            "initialize",
            "finalize",
            METHOD(&FillExtrusionLayer::getFillExtrusionOpacity, "nativeGetFillExtrusionOpacity"),
            METHOD(&FillExtrusionLayer::getFillExtrusionColor, "nativeGetFillExtrusionColor"),
            METHOD(&FillExtrusionLayer::getFillExtrusionTranslate, "nativeGetFillExtrusionTranslate"),
            METHOD(&FillExtrusionLayer::getFillExtrusionTranslateAnchor, "nativeGetFillExtrusionTranslateAnchor"),
            METHOD(&FillExtrusionLayer::getFillExtrusionPattern, "nativeGetFillExtrusionPattern"),
            METHOD(&FillExtrusionLayer::getFillExtrusionHeight, "nativeGetFillExtrusionHeight"),
            METHOD(&FillExtrusionLayer::getFillExtrusionBase, "nativeGetFillExtrusionBase"));
    }

} // namespace android
} // namespace mbgl
