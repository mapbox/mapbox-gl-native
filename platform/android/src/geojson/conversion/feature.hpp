#pragma once

#include "../../conversion/conversion.hpp"
#include "../feature.hpp"

#include <mbgl/util/feature.hpp>
#include <jni/jni.hpp>

#include <vector>
#include <unordered_map>

namespace mbgl {
namespace android {
namespace conversion {

template <>
struct Converter<jni::jobject*, std::unordered_map<std::string, mbgl::Value>> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const std::unordered_map<std::string, mbgl::Value>& value) const;
};

template <>
struct Converter<jni::Object<android::geojson::Feature>, mbgl::Feature> {
    Result<jni::Object<android::geojson::Feature>> operator()(jni::JNIEnv& env, const mbgl::Feature& value) const;
};

template <>
struct Converter<jni::Array<jni::Object<android::geojson::Feature>>, std::vector<mbgl::Feature>> {
    Result<jni::Array<jni::Object<android::geojson::Feature>>> operator()(jni::JNIEnv& env, const std::vector<mbgl::Feature>& value) const;
};

} // namespace conversion
} // namespace android
} // namespace mbgl
