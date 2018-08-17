#pragma once

#include "../../conversion/conversion.hpp"
#include "../feature.hpp"

#include <mbgl/util/feature.hpp>
#include <jni/jni.hpp>

#include <vector>

namespace mbgl {
namespace android {
namespace conversion {

template <>
struct Converter<jni::Array<jni::Object<android::geojson::Feature>>, std::vector<mbgl::Feature>> {
    Result<jni::Array<jni::Object<android::geojson::Feature>>> operator()(jni::JNIEnv& env, const std::vector<mbgl::Feature>& value) const;
};

} // namespace conversion
} // namespace android
} // namespace mbgl
