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
struct Converter<jni::Object<android::gson::JsonElement>, mbgl::style::PropertyExpression<T>> {

    Result<jni::Object<android::gson::JsonElement>> operator()(jni::JNIEnv& env, const mbgl::style::PropertyExpression<T>& value) const {
        // Convert expressions
        mbgl::Value expressionValue = value.getExpression().serialize();
        return gson::JsonElement::New(env, expressionValue);
    }
};

} // namespace conversion
} // namespace android
} // namespace mbgl
