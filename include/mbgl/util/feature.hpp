#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/string.hpp>

#include <mapbox/feature.hpp>

namespace mbgl {

using Value = mapbox::feature::value;
using NullValue = mapbox::feature::null_value_t;
using PropertyMap = mapbox::feature::property_map;
using FeatureIdentifier = mapbox::feature::identifier;
using Feature = mapbox::feature::feature<double>;
using FeatureState = PropertyMap;
using FeatureStates = std::unordered_map<std::string, FeatureState>; // <featureID, FeatureState>
using LayerFeatureStates = std::unordered_map<std::string, FeatureStates>; // <sourceLayer, FeatureStates>

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

inline optional<std::string> featureIDtoString(const FeatureIdentifier& id) {
    if (id.is<NullValue>()) {
        return nullopt;
    }

    return id.match(
        [] (const std::string& value_) {
            return value_;
        },
        [] (uint64_t value_) {
            return util::toString(value_);
        },
        [] (int64_t value_) {
            return util::toString(value_);
        },
        [] (double value_) {
            return util::toString(value_);
        },
        [] (const auto&) -> optional<std::string> {
            return nullopt;
        });
}

} // namespace mbgl
