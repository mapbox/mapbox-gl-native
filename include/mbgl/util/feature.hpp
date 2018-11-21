#pragma once

#include <mbgl/util/optional.hpp>

#include <mapbox/feature.hpp>

namespace mbgl {

using Value = mapbox::feature::value;
using NullValue = mapbox::feature::null_value_t;
using PropertyMap = mapbox::feature::property_map;
using FeatureIdentifier = mapbox::feature::identifier;
using Feature = mapbox::feature::feature<double>;

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
