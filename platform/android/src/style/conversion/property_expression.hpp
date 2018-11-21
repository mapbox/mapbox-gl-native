#pragma once

#include "../../conversion/conversion.hpp"
#include "../../gson/json_element.hpp"

#include <mbgl/style/property_expression.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace conversion {

template <class T>
struct Converter<jni::Local<jni::Object<android::gson::JsonElement>>, mbgl::style::PropertyExpression<T>> {
    Result<jni::Local<jni::Object<android::gson::JsonElement>>> operator()(jni::JNIEnv& env, const mbgl::style::PropertyExpression<T>& value) const {
        return gson::JsonElement::New(env, value.getExpression().serialize());
    }
};

} // namespace conversion
} // namespace android
} // namespace mbgl
