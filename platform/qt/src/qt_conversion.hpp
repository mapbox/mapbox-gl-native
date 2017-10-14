#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/optional.hpp>
#include <QVariant>

namespace mbgl {
namespace style {
namespace conversion {

template <class T, class...Args>
optional<T> convert(const QVariant& value, Error& error, Args&&...args) {
    return convert<T>(Value(value), error, std::forward<Args>(args)...);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
