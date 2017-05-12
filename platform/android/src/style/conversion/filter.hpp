#pragma once

#include "../android_conversion.hpp"
#include "../filter.hpp"
#include "../../conversion/conversion.hpp"
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/filter.hpp>

#include <jni/jni.hpp>

#include <tuple>
#include <map>

namespace mbgl {
namespace android {
namespace conversion {

inline optional<mbgl::style::Filter> toFilter(jni::JNIEnv& env, jni::Array<jni::Object<>> jfilter) {
    mbgl::optional<mbgl::style::Filter> filter;
    if (jfilter) {
      Value filterValue(env, jfilter);
      mbgl::style::conversion::Error error;
      auto converted = mbgl::style::conversion::convert<mbgl::style::Filter>(filterValue, error);
      if (!converted) {
          mbgl::Log::Error(mbgl::Event::JNI, "Error converting filter: " + error.message);
      }
      filter = std::move(*converted);
    }
    return filter;
}

template <>
struct Converter<jni::Object<mbgl::android::style::Filter>, mbgl::style::Filter> {
    Result<jni::Object<mbgl::android::style::Filter>> operator()(jni::JNIEnv& env, const mbgl::style::Filter& value) const {
        using namespace mbgl::android::style;
        auto filter = Filter::fromFilter(env, value);

        return filter;
    }
};

} // namespace conversion
} // namespace android
} // namespace mbgl