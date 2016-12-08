// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include "circle_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"

namespace mbgl {
namespace android {

    CircleLayer::CircleLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::CircleLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
    }

    CircleLayer::CircleLayer(mbgl::Map& map, mbgl::style::CircleLayer& coreLayer)
        : Layer(map, coreLayer) {
    }

    CircleLayer::~CircleLayer() = default;

    // Property getters

    jni::Object<jni::ObjectTag> CircleLayer::getCircleRadius(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleRadius());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleBlur(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleBlur());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleOpacity());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleTranslate(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleTranslate());
        return jni::Object<jni::ObjectTag>(*converted);
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

    jni::Object<jni::ObjectTag> CircleLayer::getCircleStrokeWidth(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleStrokeWidth());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleStrokeColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleStrokeColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> CircleLayer::getCircleStrokeOpacity(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::CircleLayer>()->CircleLayer::getCircleStrokeOpacity());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Class<CircleLayer> CircleLayer::javaClass;

    jni::jobject* CircleLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = CircleLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return CircleLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void CircleLayer::registerNative(jni::JNIEnv& env) {
        //Lookup the class
        CircleLayer::javaClass = *jni::Class<CircleLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<CircleLayer>(
            env, CircleLayer::javaClass, "nativePtr",
            std::make_unique<CircleLayer, JNIEnv&, jni::String, jni::String>,
            "initialize",
            "finalize",
            METHOD(&CircleLayer::getCircleRadius, "nativeGetCircleRadius"),
            METHOD(&CircleLayer::getCircleColor, "nativeGetCircleColor"),
            METHOD(&CircleLayer::getCircleBlur, "nativeGetCircleBlur"),
            METHOD(&CircleLayer::getCircleOpacity, "nativeGetCircleOpacity"),
            METHOD(&CircleLayer::getCircleTranslate, "nativeGetCircleTranslate"),
            METHOD(&CircleLayer::getCircleTranslateAnchor, "nativeGetCircleTranslateAnchor"),
            METHOD(&CircleLayer::getCirclePitchScale, "nativeGetCirclePitchScale"),
            METHOD(&CircleLayer::getCircleStrokeWidth, "nativeGetCircleStrokeWidth"),
            METHOD(&CircleLayer::getCircleStrokeColor, "nativeGetCircleStrokeColor"),
            METHOD(&CircleLayer::getCircleStrokeOpacity, "nativeGetCircleStrokeOpacity"));
    }

} // namespace android
} // namespace mbgl
