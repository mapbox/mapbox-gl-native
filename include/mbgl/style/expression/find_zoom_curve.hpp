#pragma once

#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/style/expression/step.hpp>

#include <mbgl/util/variant.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace expression {

optional<variant<InterpolateBase*, Step*>> findZoomCurve(expression::Expression* e);

} // namespace expression
} // namespace style
} // namespace mbgl
