#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/rapidjson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

Value makeValue(const JSValue*);

template <class T, class...Args>
optional<T> convert(const JSValue& value, Error& error, Args&&...args) {
    return convert<T>(makeValue(&value), error, std::forward<Args>(args)...);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
