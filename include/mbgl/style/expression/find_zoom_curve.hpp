#pragma once

#include <mbgl/style/expression/step.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/let.hpp>
#include <mbgl/style/expression/coalesce.hpp>

#include <mbgl/util/variant.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace expression {

template <typename T>
optional<variant<Interpolate<T>*, Step*>> findZoomCurve(expression::Expression* e) {
    if (auto curve = dynamic_cast<Interpolate<T>*>(e)) {
        auto z = dynamic_cast<CompoundExpressionBase*>(curve->getInput().get());
        if (z && z->getName() == "zoom") {
            return {curve};
        } else {
            return optional<variant<Interpolate<T>*, Step*>>();
        }
    } else if (auto step = dynamic_cast<Step*>(e)) {
        auto z = dynamic_cast<CompoundExpressionBase*>(step->getInput().get());
        if (z && z->getName() == "zoom") {
            return {step};
        } else {
            return optional<variant<Interpolate<T>*, Step*>>();
        }
    } else if (auto let = dynamic_cast<Let*>(e)) {
        return findZoomCurve<T>(let->getResult());
    } else if (auto coalesce = dynamic_cast<Coalesce*>(e)) {
        std::size_t length = coalesce->getLength();
        for (std::size_t i = 0; i < length; i++) {
            optional<variant<Interpolate<T>*, Step*>> childInterpolate = findZoomCurve<T>(coalesce->getChild(i));
            if (!childInterpolate) {
                continue;
            } else {
                return childInterpolate;
            }
        }
    }

    return optional<variant<Interpolate<T>*, Step*>>();
}

} // namespace expression
} // namespace style
} // namespace mbgl
