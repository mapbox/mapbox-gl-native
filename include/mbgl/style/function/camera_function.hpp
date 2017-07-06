#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/curve.hpp>
#include <mbgl/style/expression/value.hpp>
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
    using Curve = expression::Curve<typename expression::ValueConverter<T>::ExpressionType>;
    
    using Stops = std::conditional_t<
        util::Interpolatable<T>::value,
        variant<
            ExponentialStops<T>,
            IntervalStops<T>>,
        variant<
            IntervalStops<T>>>;
    
    CameraFunction(std::unique_ptr<expression::Expression> expression_)
        : expression(std::move(expression_)),
          zoomCurve(*Curve::findZoomCurve(expression.get()))
    {
        assert(!expression->isZoomConstant());
        assert(expression->isFeatureConstant());
    }

    CameraFunction(Stops stops_)
        : stops(std::move(stops_)),
          expression(stops.match([&] (const auto& s) {
            return expression::Convert::toExpression(s);
          })),
          zoomCurve(*Curve::findZoomCurve(expression.get()))
    {}

    T evaluate(float zoom) const {
        const expression::EvaluationResult result = expression->evaluate(expression::EvaluationParameters(zoom, nullptr));
        if (result) {
           const optional<T> typed = expression::fromExpressionValue<T>(*result);
           return typed ? *typed : T();
        }
        return T();
    }
    
    float interpolationFactor(const Range<float>& inputLevels, const float& inputValue) const {
        return zoomCurve->interpolationFactor(Range<double> { inputLevels.min, inputLevels.max }, inputValue);
    }
    
    Range<float> getCoveringStops(const float lower, const float upper) const {
        return zoomCurve->getCoveringStops(lower, upper);
    }
    
    friend bool operator==(const CameraFunction& lhs,
                           const CameraFunction& rhs) {
        return lhs.stops == rhs.stops;
    }

    bool useIntegerZoom = false;
    
    // retained for compatibility with pre-expression function API
    Stops stops;

private:
    std::shared_ptr<expression::Expression> expression;
    const Curve* zoomCurve;
};

} // namespace style
} // namespace mbgl
