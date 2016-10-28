#pragma once

#include <mbgl/style/function/source_function.hpp>
#include <mbgl/style/function/composite_function.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

class GeometryTileFeature;

namespace style {

template <class T>
class PossiblyEvaluatedPropertyValue {
private:
    using Value = variant<
        T,
        SourceFunction<T>,
        CompositeFunction<T>>;

    Value value;

public:
    PossiblyEvaluatedPropertyValue() = default;
    PossiblyEvaluatedPropertyValue(Value v) : value(std::move(v)) {}

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

    T evaluate(float z, const GeometryTileFeature& feature) const {
        return value.match(
            [&] (const T& t) { return t; },
            [&] (const SourceFunction<T>& t) { return t.evaluate(feature); },
            [&] (const CompositeFunction<T>& t) { return t.evaluate(z, feature); });
    }
};

} // namespace style

namespace util {

template <typename T>
struct Interpolator<style::PossiblyEvaluatedPropertyValue<T>> {
    style::PossiblyEvaluatedPropertyValue<T> operator()(const style::PossiblyEvaluatedPropertyValue<T>& a,
                                                        const style::PossiblyEvaluatedPropertyValue<T>& b,
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
