#include <mbgl/style/expression/find_zoom_curve.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/let.hpp>
#include <mbgl/style/expression/coalesce.hpp>
#include <mbgl/style/expression/is_constant.hpp>

#include <mbgl/util/variant.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace expression {

optional<variant<const Interpolate*, const Step*, ParsingError>> findZoomCurve(const expression::Expression* e) {
    optional<variant<const Interpolate*, const Step*, ParsingError>> result;
    
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
    } else if (auto curve = dynamic_cast<const Interpolate*>(e)) {
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
        optional<variant<const Interpolate*, const Step*, ParsingError>> childResult(findZoomCurve(&child));
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

variant<std::nullptr_t, const Interpolate*, const Step*> findZoomCurveChecked(const expression::Expression* e) {
    if (isZoomConstant(*e)) {
        return nullptr;
    }
    return findZoomCurve(e)->match(
        [](const ParsingError&) -> variant<std::nullptr_t, const Interpolate*, const Step*> {
            assert(false);
            return nullptr;
        },
        [](auto zoomCurve) -> variant<std::nullptr_t, const Interpolate*, const Step*> {
            return zoomCurve;
        }
    );
}

} // namespace expression
} // namespace style
} // namespace mbgl
