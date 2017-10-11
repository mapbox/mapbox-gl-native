#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

#include <mbgl/util/optional.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace conversion {

Value makeValue(const v8::Local<v8::Value>);

template <class T, class...Args>
optional<T> convert(const v8::Local<v8::Value>& value, Error& error, Args&&...args) {
    return convert<T>(makeValue(value), error, std::forward<Args>(args)...);
}


} // namespace conversion
} // namespace style
} // namespace mbgl
