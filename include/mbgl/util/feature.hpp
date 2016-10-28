#pragma once

#include <mbgl/util/optional.hpp>

#include <mapbox/geometry/feature.hpp>

namespace mbgl {

using Value = mapbox::geometry::value;
using NullValue = mapbox::geometry::null_value_t;
using PropertyMap = mapbox::geometry::property_map;
using FeatureIdentifier = mapbox::geometry::identifier;
using Feature = mapbox::geometry::feature<double>;

template <class T>
optional<T> numericValue(const Value& value) {
    return value.match(
        [] (uint64_t t) {
            return optional<T>(t);
        },
        [] (int64_t t) {
            return optional<T>(t);
        },
        [] (double t) {
            return optional<T>(t);
        },
        [] (auto) {
            return optional<T>();
        });
}

} // namespace mbgl
