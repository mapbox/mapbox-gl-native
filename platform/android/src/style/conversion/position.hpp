#pragma once

#include "../../conversion/conversion.hpp"
#include "../position.hpp"

#include <mbgl/style/position.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace conversion {

template <>
struct Converter<jni::Local<jni::Object<Position>>, mbgl::style::Position> {
    Result<jni::Local<jni::Object<Position>>> operator()(jni::JNIEnv &env, const mbgl::style::Position &value) const;
};

template <>
struct Converter<mbgl::style::Position, jni::Object<Position>> {
    Result<mbgl::style::Position> operator()(jni::JNIEnv &env, const jni::Object<Position> &value) const;
};

}
}
}
