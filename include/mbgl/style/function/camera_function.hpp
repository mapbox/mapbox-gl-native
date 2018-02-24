#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/style/expression/step.hpp>
#include <mbgl/style/expression/find_zoom_curve.hpp>
#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/function/convert.hpp>
#include <mbgl/style/function/exponential_stops.hpp>
#include <mbgl/style/function/interval_stops.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {
namespace style {

template <class T>
class CameraFunction {
public:
    using Stops = std::conditional_t<
        util::Interpolatable<T>::value,
        variant<
            ExponentialStops<T>,
            IntervalStops<T>>,
        variant<
            IntervalStops<T>>>;
    
    CameraFunction(std::unique_ptr<expression::Expression> expression_)
         : expression(std::move(expression_)),
           zoomCurve(expression::findZoomCurveChecked(expression.get()))
    {
        assert(!expression::isZoomConstant(*expression));
        assert(expression::isFeatureConstant(*expression));
    }

    CameraFunction(Stops stops_)
        : stops(std::move(stops_)),
          expression(stops.match([&] (const auto& s) {
            return expression::Convert::toExpression(s);
          })),
          zoomCurve(expression::findZoomCurveChecked(expression.get()))
    {}

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
            [&](const expression::InterpolateBase* z) {
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

    const expression::Expression& getExpression() const { return *expression; }

    // retained for compatibility with pre-expression function API
    Stops stops;

private:
    std::shared_ptr<expression::Expression> expression;
    const variant<const expression::InterpolateBase*, const expression::Step*> zoomCurve;
};

} // namespace style
} // namespace mbgl
