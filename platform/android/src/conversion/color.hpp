#pragma once

#include "conversion.hpp"

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace android {
namespace conversion {

template <>
struct Converter<mbgl::Color, int> {
    Result<mbgl::Color> operator()(jni::JNIEnv&, const int& color) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
