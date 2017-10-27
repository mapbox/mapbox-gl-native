#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <unordered_set>

namespace mbgl {
namespace style {
namespace expression {

bool isExpression(const conversion::Convertible& value);

} // namespace expression
} // namespace style
} // namespace mbgl
