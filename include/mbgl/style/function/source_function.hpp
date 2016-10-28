#pragma once

#include <mbgl/style/function/exponential_stops.hpp>
#include <mbgl/style/function/interval_stops.hpp>
#include <mbgl/style/function/categorical_stops.hpp>
#include <mbgl/style/function/identity_stops.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/variant.hpp>

#include <string>

namespace mbgl {
namespace style {

template <class T>
class SourceFunction {
public:
    using Stops = std::conditional_t<
        util::Interpolatable<T>,
        variant<
            ExponentialStops<T>,
            IntervalStops<T>,
            CategoricalStops<T>,
            IdentityStops<T>>,
        variant<
            IntervalStops<T>,
            CategoricalStops<T>,
            IdentityStops<T>>>;

    SourceFunction(std::string property_, Stops stops_)
        : property(std::move(property_)),
          stops(std::move(stops_)) {
    }

    T evaluate(const GeometryTileFeature& feature) const {
        optional<Value> v = feature.getValue(property);
        if (!v) {
            return T();
        }
        return stops.match([&] (const auto& s) {
            return s.evaluate(*v);
        });
    }

    friend bool operator==(const SourceFunction& lhs,
                           const SourceFunction& rhs) {
        return lhs.property == rhs.property && lhs.stops == rhs.stops;
    }

    std::string property;
    Stops stops;
};

} // namespace style
} // namespace mbgl
