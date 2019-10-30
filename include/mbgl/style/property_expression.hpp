#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/style/expression/step.hpp>
#include <mbgl/style/expression/find_zoom_curve.hpp>
#include <mbgl/util/range.hpp>

namespace mbgl {
namespace style {

class PropertyExpressionBase {
public:
    explicit PropertyExpressionBase(std::unique_ptr<expression::Expression>);

    bool isZoomConstant() const noexcept;
    bool isFeatureConstant() const noexcept;
    bool isRuntimeConstant() const noexcept;
    float interpolationFactor(const Range<float>&, const float) const noexcept;
    Range<float> getCoveringStops(const float, const float) const noexcept;
    const expression::Expression& getExpression() const noexcept;

    bool useIntegerZoom = false;

protected:
    std::shared_ptr<const expression::Expression> expression;
    variant<std::nullptr_t, const expression::Interpolate*, const expression::Step*> zoomCurve;
    bool isZoomConstant_;
    bool isFeatureConstant_;
    bool isRuntimeConstant_;
};

template <class T>
class PropertyExpression final : public PropertyExpressionBase {
public:
    // Second parameter to be used only for conversions from legacy functions.
    PropertyExpression(std::unique_ptr<expression::Expression> expression_, optional<T> defaultValue_ = nullopt)
        : PropertyExpressionBase(std::move(expression_)),
          defaultValue(std::move(defaultValue_)) {
    }

    T evaluate(const expression::EvaluationContext& context, T finalDefaultValue = T()) const {
        const expression::EvaluationResult result = expression->evaluate(context);
        if (result) {
            const optional<T> typed = expression::fromExpressionValue<T>(*result);
            return typed ? *typed : defaultValue ? *defaultValue : finalDefaultValue;
        }
        return defaultValue ? *defaultValue : finalDefaultValue;
    }

    T evaluate(float zoom) const {
        assert(!isZoomConstant());
        assert(isFeatureConstant());
        return evaluate(expression::EvaluationContext(zoom));
    }

    T evaluate(const GeometryTileFeature& feature, T finalDefaultValue) const {
        assert(isZoomConstant());
        assert(!isFeatureConstant());
        return evaluate(expression::EvaluationContext(&feature), finalDefaultValue);
    }

    T evaluate(const GeometryTileFeature& feature,
               const std::set<std::string>& availableImages,
               T finalDefaultValue) const {
        return evaluate(expression::EvaluationContext(&feature).withAvailableImages(&availableImages),
                        finalDefaultValue);
    }

    T evaluate(float zoom, const GeometryTileFeature& feature, T finalDefaultValue) const {
        return evaluate(expression::EvaluationContext(zoom, &feature), finalDefaultValue);
    }

    T evaluate(float zoom,
               const GeometryTileFeature& feature,
               const std::set<std::string>& availableImages,
               T finalDefaultValue) const {
        return evaluate(expression::EvaluationContext(zoom, &feature).withAvailableImages(&availableImages),
                        finalDefaultValue);
    }

    T evaluate(float zoom, const GeometryTileFeature& feature, const FeatureState& state, T finalDefaultValue) const {
        assert(!isFeatureConstant());
        return evaluate(expression::EvaluationContext(zoom, &feature, &state), finalDefaultValue);
    }

    std::vector<optional<T>> possibleOutputs() const {
        return expression::fromExpressionValues<T>(expression->possibleOutputs());
    }

    friend bool operator==(const PropertyExpression& lhs,
                           const PropertyExpression& rhs) {
        return *lhs.expression == *rhs.expression;
    }

private:
    optional<T> defaultValue;
};

} // namespace style
} // namespace mbgl
