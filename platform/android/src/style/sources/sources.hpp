#pragma once

#include <mbgl/map/map.hpp>
#include <mbgl/style/source.hpp>

#include "source.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
    
    mbgl::android::Source* initializeSourcePeer(mbgl::Map&, mbgl::style::Source&);
    
    jni::jobject* createJavaSourcePeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Source&);
    
    void registerNativeSources(jni::JNIEnv&);

}
}