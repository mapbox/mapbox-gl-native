#pragma once

#include <mbgl/util/rapidjson.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class T, class...Args>
optional<T> convert(const JSValue& value, Error& error, Args&&...args) {
    return convert<T>(Value(&value), error, std::forward<Args>(args)...);
}

} // namespace conversion
} // namespace style
} // namespace mbgl

