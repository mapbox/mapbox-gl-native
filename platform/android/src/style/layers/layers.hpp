#pragma once

#include <mbgl/map/map.hpp>
#include <mbgl/style/layer.hpp>

#include "layer.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

mbgl::android::Layer* initializeLayerPeer(mbgl::Map&, mbgl::style::Layer&);

jni::jobject* createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&);

void registerNativeLayers(jni::JNIEnv&);

}
}