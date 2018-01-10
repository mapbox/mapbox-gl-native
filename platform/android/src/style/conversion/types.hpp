// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.
#pragma once

#include "types_string_values.hpp"
#include "../../conversion/conversion.hpp"
#include "../../conversion/constant.hpp"

#include <mbgl/style/types.hpp>
#include <mbgl/util/optional.hpp>
#include <jni/jni.hpp>

#include <string>

namespace mbgl {
namespace android {
namespace conversion {

template <>
struct Converter<jni::jobject*, mbgl::style::VisibilityType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::VisibilityType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::LineCapType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::LineCapType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::LineJoinType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::LineJoinType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::SymbolPlacementType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::SymbolPlacementType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::AlignmentType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::AlignmentType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::IconTextFitType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::IconTextFitType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::SymbolAnchorType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::SymbolAnchorType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::TextJustifyType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::TextJustifyType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::TextTransformType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::TextTransformType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::TranslateAnchorType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::TranslateAnchorType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::CirclePitchScaleType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::CirclePitchScaleType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::HillshadeIlluminationAnchorType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::HillshadeIlluminationAnchorType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};

template <>
struct Converter<jni::jobject*, mbgl::style::LightAnchorType> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::LightAnchorType& value) const {
        return convert<jni::jobject*, std::string>(env, toString(value));
    }
};


} // namespace conversion
} // namespace android
} // namespace mbgl
