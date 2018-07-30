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
    
    switch (e->getKind()) {
    case Kind::Let: {
        auto let = static_cast<const Let*>(e);
        result = findZoomCurve(let->getResult());
        break;
    }
    case Kind::Coalesce: {
        auto coalesce = static_cast<const Coalesce*>(e);
        std::size_t length = coalesce->getLength();
        for (std::size_t i = 0; i < length; i++) {
            result = findZoomCurve(coalesce->getChild(i));
            if (result) {
                break;
            }
        }
        break;
    }
    case Kind::Interpolate: {
        auto curve = static_cast<const Interpolate*>(e);
        if (curve->getInput()->getKind() == Kind::CompoundExpression) {
            auto z = static_cast<CompoundExpressionBase*>(curve->getInput().get());
            if (z && z->getName() == "zoom") {
                result = {curve};
            }
        }
        break;
    }
    case Kind::Step: {
        auto step = static_cast<const Step*>(e);
        if (step->getInput()->getKind() == Kind::CompoundExpression) {
            auto z = static_cast<CompoundExpressionBase*>(step->getInput().get());
            if (z && z->getName() == "zoom") {
                result = {step};
            }
        }
        break;
    }
    default:
        break;
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
