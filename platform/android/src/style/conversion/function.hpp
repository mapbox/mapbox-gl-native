#pragma once

#include <mbgl/style/property_value.hpp>
#include "../../conversion/conversion.hpp"
#include "../../conversion/constant.hpp"
#include "types.hpp"
#include "../../java/lang.hpp"

#include <jni/jni.hpp>
#include "gson.hpp"
#include <tuple>
#include <map>

namespace mbgl {
namespace android {
namespace conversion {

template <class T>
struct Converter<jni::jobject*, mbgl::style::CameraFunction<T>> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::CameraFunction<T>& value) const {
        // Convert expressions
        mbgl::Value expressionValue = value.getExpression().serialize();
        JsonEvaluator jsonEvaluator{env};
        jni::jobject* converted = apply_visitor(jsonEvaluator, expressionValue);

        return converted;
    }
};

template <class T>
struct Converter<jni::jobject*, mbgl::style::SourceFunction<T>> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::SourceFunction<T>& value) const {
        // Convert expressions
        mbgl::Value expressionValue = value.getExpression().serialize();
        JsonEvaluator jsonEvaluator{env};
        jni::jobject* converted = apply_visitor(jsonEvaluator, expressionValue);

        return converted;
    }
};

template <class T>
struct Converter<jni::jobject*, mbgl::style::CompositeFunction<T>> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::CompositeFunction<T>& value) const {
        // Convert expressions
        mbgl::Value expressionValue = value.getExpression().serialize();
        JsonEvaluator jsonEvaluator{env};
        jni::jobject* converted = apply_visitor(jsonEvaluator, expressionValue);

        return converted;
    }
};

} // namespace conversion
} // namespace android
} // namespace mbgl
