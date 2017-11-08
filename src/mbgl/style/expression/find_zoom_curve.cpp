#include <mbgl/style/expression/find_zoom_curve.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/let.hpp>
#include <mbgl/style/expression/coalesce.hpp>

#include <mbgl/util/variant.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace expression {

optional<variant<const InterpolateBase*, const Step*, ParsingError>> findZoomCurve(const expression::Expression* e) {
    optional<variant<const InterpolateBase*, const Step*, ParsingError>> result;
    
    if (auto let = dynamic_cast<const Let*>(e)) {
        result = findZoomCurve(let->getResult());
    } else if (auto coalesce = dynamic_cast<const Coalesce*>(e)) {
        std::size_t length = coalesce->getLength();
        for (std::size_t i = 0; i < length; i++) {
            result = findZoomCurve(coalesce->getChild(i));
            if (result) {
                break;
            }
        }
    } else if (auto curve = dynamic_cast<const InterpolateBase*>(e)) {
        auto z = dynamic_cast<CompoundExpressionBase*>(curve->getInput().get());
        if (z && z->getName() == "zoom") {
            result = {curve};
        }
    } else if (auto step = dynamic_cast<const Step*>(e)) {
        auto z = dynamic_cast<CompoundExpressionBase*>(step->getInput().get());
        if (z && z->getName() == "zoom") {
            result = {step};
        }
    }
    
    if (result && result->is<ParsingError>()) {
        return result;
    }
    
    e->eachChild([&](const Expression& child) {
        optional<variant<const InterpolateBase*, const Step*, ParsingError>> childResult(findZoomCurve(&child));
        if (childResult) {
            if (childResult->is<ParsingError>()) {
                result = childResult;
            } else if (!result && childResult) {
                result = {ParsingError {
                    R"("zoom" expression may only be used as input to a top-level "step" or "interpolate" expression.)", ""
                }};
            } else if (result && childResult && result != childResult) {
                result = {ParsingError {
                    R"(Only one zoom-based "step" or "interpolate" subexpression may be used in an expression.)", ""
                }};
            }
        }
    });

    return result;
}

variant<const InterpolateBase*, const Step*> findZoomCurveChecked(const expression::Expression* e) {
    return findZoomCurve(e)->match(
        [](const ParsingError&) -> variant<const InterpolateBase*, const Step*> {
            assert(false);
            return {};
        },
        [](auto zoomCurve) -> variant<const InterpolateBase*, const Step*> {
            return {std::move(zoomCurve)};
        }
    );
}

} // namespace expression
} // namespace style
} // namespace mbgl
