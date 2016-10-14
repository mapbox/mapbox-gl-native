// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

#pragma once

#include "layer.hpp"
#include <mbgl/style/layers/terrain_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class TerrainLayer : public Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/TerrainLayer"; };

    static jni::Class<TerrainLayer> javaClass;

    static void registerNative(jni::JNIEnv&);

    TerrainLayer(jni::JNIEnv&, jni::String, jni::String);

    TerrainLayer(mbgl::Map&, mbgl::style::TerrainLayer&);

    ~TerrainLayer();

    // Property getters
    jni::Object<jni::ObjectTag> getTerrainShadowColor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTerrainHighlightColor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTerrainAccentColor(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTerrainIlluminationDirection(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTerrainIlluminationAlignment(jni::JNIEnv&);

    jni::Object<jni::ObjectTag> getTerrainExaggeration(jni::JNIEnv&);

    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class TerrainLayer

} // namespace android
} // namespace mbgl
