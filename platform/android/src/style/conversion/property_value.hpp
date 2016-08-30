#pragma once

#include <mbgl/style/property_value.hpp>
#include "../../conversion/conversion.hpp"
#include "../../conversion/constant.hpp"
#include "types.hpp"
#include "function.hpp"

namespace mbgl {
namespace android {
namespace conversion {

template <class T>
struct Converter<jni::jobject*, mbgl::style::PropertyValue<T>> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::PropertyValue<T>& value) const {

        if(value.isUndefined()) {
            //Return a nullptr representing a Java null value
            return {nullptr};
        } else if (value.isConstant()) {
            //Time to convert the constant value
            Result<jni::jobject*> result = convert<jni::jobject*, T>(env, value.asConstant());
            return {*result};
            //return converted;
        } else if (value.isFunction()) {
            //Must be a function than
            return convert<jni::jobject*, mbgl::style::Function<T>>(env, value.asFunction());
        } else {
            throw std::runtime_error("Unknown property value type");
        }

    }
};

} // namespace conversion
} // namespace android
} // namespace mbgl