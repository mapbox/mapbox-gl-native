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
class CompositeFunction {
public:
    // The second parameter should be used only for conversions from legacy functions.
    CompositeFunction(std::unique_ptr<expression::Expression> expression_, optional<T> defaultValue_ = {})
        : isExpression(defaultValue_),
          expression(std::move(expression_)),
          defaultValue(std::move(defaultValue_)),
          zoomCurve(expression::findZoomCurveChecked(expression.get())) {
        assert(!expression::isZoomConstant(*expression));
        assert(!expression::isFeatureConstant(*expression));
    }

    // Return the range obtained by evaluating the function at each of the zoom levels in zoomRange
    template <class Feature>
    Range<T> evaluate(const Range<float>& zoomRange, const Feature& feature, T finalDefaultValue) {
        return Range<T> {
            evaluate(zoomRange.min, feature, finalDefaultValue),
            evaluate(zoomRange.max, feature, finalDefaultValue)
        };
    }

    template <class Feature>
    T evaluate(float zoom, const Feature& feature, T finalDefaultValue) const {
        const expression::EvaluationResult result = expression->evaluate(expression::EvaluationContext({zoom}, &feature));
        if (result) {
            const optional<T> typed = expression::fromExpressionValue<T>(*result);
            return typed ? *typed : defaultValue ? *defaultValue : finalDefaultValue;
        }
        return defaultValue ? *defaultValue : finalDefaultValue;
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

    friend bool operator==(const CompositeFunction& lhs,
                           const CompositeFunction& rhs) {
        return *lhs.expression == *rhs.expression;
    }

    const expression::Expression& getExpression() const { return *expression; }

    bool useIntegerZoom = false;
    bool isExpression;
    
private:
    std::shared_ptr<const expression::Expression> expression;
    optional<T> defaultValue;
    const variant<const expression::Interpolate*, const expression::Step*> zoomCurve;
};

} // namespace style
} // namespace mbgl
