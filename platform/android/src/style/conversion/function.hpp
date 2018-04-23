#pragma once

#include <mbgl/style/property_value.hpp>
#include "../../conversion/conversion.hpp"
#include "../../conversion/constant.hpp"
#include "types.hpp"
#include "../../java/lang.hpp"

#include <jni/jni.hpp>
#include "../../gson/json_element.hpp"

#include <tuple>
#include <map>

namespace mbgl {
namespace android {
namespace conversion {

template <class T>
struct Converter<jni::Object<android::gson::JsonElement>, mbgl::style::CameraFunction<T>> {

    Result<jni::Object<android::gson::JsonElement>> operator()(jni::JNIEnv& env, const mbgl::style::CameraFunction<T>& value) const {
        // Convert expressions
        mbgl::Value expressionValue = value.getExpression().serialize();
        return gson::JsonElement::New(env, expressionValue);
    }
};

template <class T>
struct Converter<jni::Object<android::gson::JsonElement>, mbgl::style::SourceFunction<T>> {

    Result<jni::Object<android::gson::JsonElement>> operator()(jni::JNIEnv& env, const mbgl::style::SourceFunction<T>& value) const {
        // Convert expressions
        mbgl::Value expressionValue = value.getExpression().serialize();
        return gson::JsonElement::New(env, expressionValue);
    }
};

template <class T>
struct Converter<jni::Object<android::gson::JsonElement>, mbgl::style::CompositeFunction<T>> {

    Result<jni::Object<android::gson::JsonElement>> operator()(jni::JNIEnv& env, const mbgl::style::CompositeFunction<T>& value) const {
        // Convert expressions
        mbgl::Value expressionValue = value.getExpression().serialize();
        return gson::JsonElement::New(env, expressionValue);
    }
};

} // namespace conversion
} // namespace android
} // namespace mbgl
