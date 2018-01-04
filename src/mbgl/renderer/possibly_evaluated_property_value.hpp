#pragma once

#include <mbgl/style/function/source_function.hpp>
#include <mbgl/style/function/composite_function.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

template <class T>
class PossiblyEvaluatedPropertyValue {
private:
    using Value = variant<
        T,
        style::SourceFunction<T>,
        style::CompositeFunction<T>>;

    Value value;

public:
    PossiblyEvaluatedPropertyValue() = default;
    PossiblyEvaluatedPropertyValue(Value v, bool useIntegerZoom_ = false)
        : value(std::move(v)),
          useIntegerZoom(useIntegerZoom_) {}

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
                [&] (const style::SourceFunction<T>& function) {
                    return function.evaluate(feature, defaultValue);
                },
                [&] (const style::CompositeFunction<T>& function) {
                    if (useIntegerZoom) {
                        return function.evaluate(floor(zoom), feature, defaultValue);
                    } else {
                        return function.evaluate(zoom, feature, defaultValue);
                    }
                }
        );
    }

    bool useIntegerZoom;
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
