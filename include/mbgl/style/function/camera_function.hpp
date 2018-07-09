#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/style/expression/step.hpp>
#include <mbgl/style/expression/find_zoom_curve.hpp>
#include <mbgl/util/range.hpp>

namespace mbgl {
namespace style {

template <class T>
class CameraFunction {
public:
    // The second parameter should be used only for conversions from legacy functions.
    CameraFunction(std::unique_ptr<expression::Expression> expression_, bool isExpression_ = true)
         : isExpression(isExpression_),
           expression(std::move(expression_)),
           zoomCurve(expression::findZoomCurveChecked(expression.get())) {
        assert(!expression::isZoomConstant(*expression));
        assert(expression::isFeatureConstant(*expression));
    }

    T evaluate(float zoom) const {
        const expression::EvaluationResult result = expression->evaluate(expression::EvaluationContext(zoom, nullptr));
        if (result) {
           const optional<T> typed = expression::fromExpressionValue<T>(*result);
           return typed ? *typed : T();
        }
        return T();
    }
    
    float interpolationFactor(const Range<float>& inputLevels, const float inputValue) const {
        return zoomCurve.match(
            [&](const expression::Interpolate* z) {
                return z->interpolationFactor(Range<double> { inputLevels.min, inputLevels.max }, inputValue);
            },
            [&](const expression::Step*) { return 0.0f; }
        );
    }
    
    Range<float> getCoveringStops(const float lower, const float upper) const {
        return zoomCurve.match(
            [&](auto z) { return z->getCoveringStops(lower, upper); }
        );
    }

    std::vector<optional<T>> possibleOutputs() const {
        return expression::fromExpressionValues<T>(expression->possibleOutputs());
    }

    friend bool operator==(const CameraFunction& lhs,
                           const CameraFunction& rhs) {
        return *lhs.expression == *rhs.expression;
    }

    bool useIntegerZoom = false;
    bool isExpression;

    const expression::Expression& getExpression() const { return *expression; }

private:
    std::shared_ptr<const expression::Expression> expression;
    const variant<const expression::Interpolate*, const expression::Step*> zoomCurve;
};

} // namespace style
} // namespace mbgl
