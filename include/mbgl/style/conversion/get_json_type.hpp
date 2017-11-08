#pragma once

#include <mbgl/style/conversion.hpp>
#include <string>

namespace mbgl {
namespace style {
namespace conversion {

std::string getJSONType(const Convertible& value);

} // namespace conversion
} // namespace style
} // namespace mbgl
