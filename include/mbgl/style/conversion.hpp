#pragma once

#include <string>

namespace mbgl {
namespace style {
namespace conversion {

// This is a forward-declaration only header intended to minimize dependencies and to improve
// compilation speed. In order to specialize implementations and get access to the actual
// implementation, include <mbgl/style/conversion_impl.hpp>.

struct Error { std::string message; };

template <typename T>
class ConversionTraits;

class Convertible;

template <typename T, typename Enable = void>
struct Converter;

template <typename T, typename Enable = void>
struct ValueFactory;

} // namespace conversion
} // namespace style
} // namespace mbgl

