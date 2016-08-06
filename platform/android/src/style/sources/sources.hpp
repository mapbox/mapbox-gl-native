#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/optional.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

    mbgl::optional<std::unique_ptr<mbgl::style::Source>> convertToNativeSource(jni::JNIEnv& env, jni::Object<jni::jobject> jsource, jni::String id);

}
}