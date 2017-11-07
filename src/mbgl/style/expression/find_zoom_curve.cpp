#include <mbgl/style/expression/find_zoom_curve.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/let.hpp>
#include <mbgl/style/expression/coalesce.hpp>

#include <mbgl/util/variant.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace expression {

optional<variant<InterpolateBase*, Step*>> findZoomCurve(expression::Expression* e) {
    if (auto curve = dynamic_cast<InterpolateBase*>(e)) {
        auto z = dynamic_cast<CompoundExpressionBase*>(curve->getInput().get());
        if (z && z->getName() == "zoom") {
            return {curve};
        } else {
            return optional<variant<InterpolateBase*, Step*>>();
        }
    } else if (auto step = dynamic_cast<Step*>(e)) {
        auto z = dynamic_cast<CompoundExpressionBase*>(step->getInput().get());
        if (z && z->getName() == "zoom") {
            return {step};
        } else {
            return optional<variant<InterpolateBase*, Step*>>();
        }
    } else if (auto let = dynamic_cast<Let*>(e)) {
        return findZoomCurve(let->getResult());
    } else if (auto coalesce = dynamic_cast<Coalesce*>(e)) {
        std::size_t length = coalesce->getLength();
        for (std::size_t i = 0; i < length; i++) {
            optional<variant<InterpolateBase*, Step*>> childInterpolate = findZoomCurve(coalesce->getChild(i));
            if (!childInterpolate) {
                continue;
            } else {
                return childInterpolate;
            }
        }
    }

    return optional<variant<InterpolateBase*, Step*>>();
}

} // namespace expression
} // namespace style
} // namespace mbgl
