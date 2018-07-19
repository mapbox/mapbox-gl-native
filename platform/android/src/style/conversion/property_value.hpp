#pragma once

#include <mbgl/style/color_ramp_property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>
#include <mbgl/style/property_value.hpp>
#include "../../conversion/conversion.hpp"
#include "../../conversion/constant.hpp"
#include "property_expression.hpp"
#include "types.hpp"

namespace mbgl {
namespace android {
namespace conversion {

/**
 * Conversion from core property value types to Java property value types
 */
template <typename T>
class PropertyValueEvaluator {
public:

    PropertyValueEvaluator(jni::JNIEnv& _env) : env(_env) {}

    jni::jobject* operator()(const mbgl::style::Undefined) const {
        return nullptr;
    }

    jni::jobject* operator()(const T &value) const {
        Result<jni::jobject*> result = convert<jni::jobject*>(env, value);
        return *result;
    }

    jni::jobject* operator()(const mbgl::style::PropertyExpression<T> &value) const {
        return *convert<jni::Object<android::gson::JsonElement>, mbgl::style::PropertyExpression<T>>(env, value);
    }

private:
    jni::JNIEnv& env;

};

/**
 * Convert core property values to java
 */
template <class T>
struct Converter<jni::jobject*, mbgl::style::PropertyValue<T>> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::PropertyValue<T>& value) const {
        PropertyValueEvaluator<T> evaluator(env);
        return value.evaluate(evaluator);
    }
};

/**
 * Convert core data driven property values to java
 */
template <class T>
struct Converter<jni::jobject*, mbgl::style::DataDrivenPropertyValue<T>> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::DataDrivenPropertyValue<T>& value) const {
        PropertyValueEvaluator<T> evaluator(env);
        return value.evaluate(evaluator);
    }
};

/**
 * Convert core heat map color property value to java
 */
template <>
struct Converter<jni::jobject*, mbgl::style::ColorRampPropertyValue> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::ColorRampPropertyValue value) const {
        PropertyValueEvaluator<mbgl::style::ColorRampPropertyValue> evaluator(env);
        return *convert<jni::jobject*>(env, value.evaluate(evaluator));
    }
};

} // namespace conversion
} // namespace android
} // namespace mbgl
