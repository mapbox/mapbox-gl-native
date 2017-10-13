#pragma once

#include "value.hpp"

#include <mbgl/util/feature.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/util/optional.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class T, class...Args>
optional<T> convert(const mbgl::android::Value& value, Error& error, Args&&...args) {
    return convert<T>(Value(value), error, std::forward<Args>(args)...);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
