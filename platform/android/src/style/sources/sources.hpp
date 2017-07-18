#pragma once

#include <mbgl/style/source.hpp>

#include "source.hpp"
#include "../../android_renderer_frontend.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

    jni::jobject* createJavaSourcePeer(jni::JNIEnv&, AndroidRendererFrontend&, mbgl::style::Source&);

    void registerNativeSources(jni::JNIEnv&);

} // namespace android
} // namespace mbgl
