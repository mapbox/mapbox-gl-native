#pragma once

#include <mbgl/util/optional.hpp>

#include <mapbox/geometry/feature.hpp>

namespace mbgl {

using Value = mapbox::geometry::value;
using NullValue = mapbox::geometry::null_value_t;
using PropertyMap = mapbox::geometry::property_map;
using FeatureIdentifier = mapbox::geometry::identifier;
using Feature = mapbox::geometry::feature<double>;

} // namespace mbgl
