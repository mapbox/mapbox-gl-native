#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/util/range.hpp>
#include <memory>
#include <map>

namespace mbgl {
namespace style {
namespace expression {

// Return the smallest range of stops that covers the interval [lower, upper]
Range<float> getCoveringStops(const std::map<double, std::unique_ptr<Expression>>& stops,
                              const double lower, const double upper);

} // namespace expression
} // namespace style
} // namespace mbgl
