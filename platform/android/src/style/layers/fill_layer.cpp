// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "fill_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"

namespace mbgl {
namespace android {

    FillLayer::FillLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::FillLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
    }

    FillLayer::FillLayer(mbgl::Map& map, mbgl::style::FillLayer& coreLayer)
        : Layer(map, coreLayer) {
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

    jni::Object<jni::ObjectTag> FillLayer::getFillColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillLayer>()->FillLayer::getFillColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> FillLayer::getFillOutlineColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillLayer>()->FillLayer::getFillOutlineColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> FillLayer::getFillTranslate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::FillLayer>()->FillLayer::getFillTranslate());
        return jni::Object<jni::ObjectTag>(*converted);
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

    jni::Class<FillLayer> FillLayer::javaClass;

    jni::jobject* FillLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = FillLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return FillLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void FillLayer::registerNative(jni::JNIEnv& env) {
        //Lookup the class
        FillLayer::javaClass = *jni::Class<FillLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<FillLayer>(
            env, FillLayer::javaClass, "nativePtr",
            std::make_unique<FillLayer, JNIEnv&, jni::String, jni::String>,
            "initialize",
            "finalize",
            METHOD(&FillLayer::getFillAntialias, "nativeGetFillAntialias"),
            METHOD(&FillLayer::getFillOpacity, "nativeGetFillOpacity"),
            METHOD(&FillLayer::getFillColor, "nativeGetFillColor"),
            METHOD(&FillLayer::getFillOutlineColor, "nativeGetFillOutlineColor"),
            METHOD(&FillLayer::getFillTranslate, "nativeGetFillTranslate"),
            METHOD(&FillLayer::getFillTranslateAnchor, "nativeGetFillTranslateAnchor"),
            METHOD(&FillLayer::getFillPattern, "nativeGetFillPattern"));
    }

} // namespace android
} // namespace mbgl
