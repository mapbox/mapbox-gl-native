#pragma once

#include <mbgl/style/property_expression.hpp>
#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

template <class T>
class PossiblyEvaluatedPropertyValue {
private:
    using Value = variant<
        T,
        style::PropertyExpression<T>>;

    Value value;

public:
    PossiblyEvaluatedPropertyValue() = default;
    PossiblyEvaluatedPropertyValue(Value v)
        : value(std::move(v)) {}

    bool isConstant() const {
        return value.template is<T>();
    }

    optional<T> constant() const {
        return value.match(
            [&] (const T& t) { return optional<T>(t); },
            [&] (const auto&) { return optional<T>(); });
    }

    T constantOr(const T& t) const {
        return constant().value_or(t);
    }

    template <class... Ts>
    auto match(Ts&&... ts) const {
        return value.match(std::forward<Ts>(ts)...);
    }

    template <class Feature>
    T evaluate(const Feature& feature, float zoom, T defaultValue) const {
        return this->match(
            [&] (const T& constant_) { return constant_; },
            [&] (const style::PropertyExpression<T>& expression) {
                return expression.evaluate(zoom, feature, defaultValue);
            }
        );
    }

    template <class Feature>
    T evaluate(const Feature& feature, float zoom, const FeatureState& featureState, T defaultValue) const {
        return this->match([&](const T& constant_) { return constant_; },
                           [&](const style::PropertyExpression<T>& expression) {
                               return expression.evaluate(zoom, feature, featureState, defaultValue);
                           });
    }
};

template <class T>
class PossiblyEvaluatedPropertyValue<Faded<T>> {
private:
    using Value = variant<
        Faded<T>,
        style::PropertyExpression<T>>;

    Value value;

public:
    PossiblyEvaluatedPropertyValue() = default;
    PossiblyEvaluatedPropertyValue(Value v)
        : value(std::move(v)) {}

    bool isConstant() const {
        return value.template is<Faded<T>>();
    }

    optional<Faded<T>> constant() const {
        return value.match(
            [&] (const Faded<T>& t) { return optional<Faded<T>>(t); },
            [&] (const auto&) { return optional<Faded<T>>(); });
    }

    Faded<T> constantOr(const Faded<T>& t) const {
        return constant().value_or(t);
    }

    template <class... Ts>
    auto match(Ts&&... ts) const {
        return value.match(std::forward<Ts>(ts)...);
    }

    template <class Feature>
    Faded<T> evaluate(const Feature& feature,
                      float zoom,
                      const std::set<std::string>& availableImages,
                      T defaultValue) const {
        return this->match(
            [&] (const Faded<T>& constant_) { return constant_; },
            [&] (const style::PropertyExpression<T>& expression) {
                if (!expression.isZoomConstant()) {
                    const T min = expression.evaluate(floor(zoom), feature, availableImages, defaultValue);
                    const T max = expression.evaluate(floor(zoom) + 1, feature, availableImages, defaultValue);
                    return Faded<T> {min, max};
                } else {
                    const T evaluated = expression.evaluate(feature, availableImages, defaultValue);
                    return Faded<T> {evaluated, evaluated};
                }
            }
        );
    }
};


namespace util {

template <typename T>
struct Interpolator<PossiblyEvaluatedPropertyValue<T>> {
    PossiblyEvaluatedPropertyValue<T> operator()(const PossiblyEvaluatedPropertyValue<T>& a,
                                                 const PossiblyEvaluatedPropertyValue<T>& b,
                                                 const double t) const {
        if (a.isConstant() && b.isConstant()) {
            return { interpolate(*a.constant(), *b.constant(), t) };
        } else {
            return { a };
        }
    }
};

} // namespace util

} // namespace mbgl
