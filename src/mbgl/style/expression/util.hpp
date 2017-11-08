#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {
namespace expression {

Result<Color> rgba(double r, double g, double b, double a);

} // namespace expression
} // namespace style
} // namespace mbgl
