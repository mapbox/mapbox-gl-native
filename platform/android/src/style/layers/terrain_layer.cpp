// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

#include "terrain_layer.hpp"

#include <string>

#include "../conversion/property_value.hpp"

namespace mbgl {
namespace android {

    TerrainLayer::TerrainLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::TerrainLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
    }

    TerrainLayer::TerrainLayer(mbgl::Map& map, mbgl::style::TerrainLayer& coreLayer)
        : Layer(map, coreLayer) {
    }

    TerrainLayer::~TerrainLayer() = default;

    // Property getters

    jni::Object<jni::ObjectTag> TerrainLayer::getTerrainShadowColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::TerrainLayer>()->TerrainLayer::getTerrainShadowColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> TerrainLayer::getTerrainHighlightColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::TerrainLayer>()->TerrainLayer::getTerrainHighlightColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> TerrainLayer::getTerrainAccentColor(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::TerrainLayer>()->TerrainLayer::getTerrainAccentColor());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> TerrainLayer::getTerrainIlluminationDirection(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::TerrainLayer>()->TerrainLayer::getTerrainIlluminationDirection());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> TerrainLayer::getTerrainIlluminationAlignment(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::TerrainLayer>()->TerrainLayer::getTerrainIlluminationAlignment());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Object<jni::ObjectTag> TerrainLayer::getTerrainExaggeration(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        Result<jni::jobject*> converted = convert<jni::jobject*>(env, layer.as<mbgl::style::TerrainLayer>()->TerrainLayer::getTerrainExaggeration());
        return jni::Object<jni::ObjectTag>(*converted);
    }

    jni::Class<TerrainLayer> TerrainLayer::javaClass;

    jni::jobject* TerrainLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = TerrainLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return TerrainLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void TerrainLayer::registerNative(jni::JNIEnv& env) {
        //Lookup the class
        TerrainLayer::javaClass = *jni::Class<TerrainLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<TerrainLayer>(
            env, TerrainLayer::javaClass, "nativePtr",
            std::make_unique<TerrainLayer, JNIEnv&, jni::String, jni::String>,
            "initialize",
            "finalize",
            METHOD(&TerrainLayer::getTerrainShadowColor, "nativeGetTerrainShadowColor"),
            METHOD(&TerrainLayer::getTerrainHighlightColor, "nativeGetTerrainHighlightColor"),
            METHOD(&TerrainLayer::getTerrainAccentColor, "nativeGetTerrainAccentColor"),
            METHOD(&TerrainLayer::getTerrainIlluminationDirection, "nativeGetTerrainIlluminationDirection"),
            METHOD(&TerrainLayer::getTerrainIlluminationAlignment, "nativeGetTerrainIlluminationAlignment"),
            METHOD(&TerrainLayer::getTerrainExaggeration, "nativeGetTerrainExaggeration"));
    }

} // namespace android
} // namespace mbgl
