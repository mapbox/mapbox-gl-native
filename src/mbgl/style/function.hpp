#ifndef MBGL_STYLE_FUNCTION
#define MBGL_STYLE_FUNCTION

#include <mbgl/style/types.hpp>
#include <mbgl/util/chrono.hpp>

#include <mapbox/optional.hpp>

#include <vector>
#include <utility>

namespace mbgl {

class StyleCalculationParameters;

template <typename T>
class Function {
public:
    using Stop = std::pair<float, T>;
    using Stops = std::vector<Stop>;

    // TODO: make explicit
    /* explicit */ Function(const T& constant)
        : stops({{ 0, constant }}) {}

    explicit Function(const Stops& stops_, float base_)
        : base(base_), stops(stops_) {}

    T evaluate(const StyleCalculationParameters&) const;

private:
    const float base = 1;
    const std::vector<std::pair<float, T>> stops;
};

// Partial specialization for cross-faded properties (*-pattern, line-dasharray).
template <typename T>
class Function<Faded<T>> {
public:
    using Stop = std::pair<float, T>;
    using Stops = std::vector<Stop>;

    // TODO: make explicit
    /* explicit */ Function(const T& constant)
        : stops({{ 0, constant }}) {}

    explicit Function(const Stops& stops_)
        : stops(stops_) {}

    Faded<T> evaluate(const StyleCalculationParameters&) const;

private:
    const std::vector<std::pair<float, T>> stops;
};

}

#endif
