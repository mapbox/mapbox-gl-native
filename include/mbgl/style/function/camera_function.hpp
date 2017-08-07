#pragma once

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

    CameraFunction(Stops stops_)
        : stops(std::move(stops_)) {
    }

    T evaluate(float zoom) const {
        return stops.match([&] (const auto& s) {
            return s.evaluate(zoom).value_or(T());
        });
    }
    
    friend bool operator==(const CameraFunction& lhs,
                           const CameraFunction& rhs) {
        return lhs.stops == rhs.stops;
    }

    Stops stops;
    bool useIntegerZoom = false;
};

} // namespace style
} // namespace mbgl
