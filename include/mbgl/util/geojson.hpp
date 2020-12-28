#pragma once

#include <mapbox/geojson.hpp>
#include <mbgl/util/feature.hpp>

namespace mbgl {

using GeoJSON = mapbox::geojson::geojson;
using FeatureCollection = mapbox::geojson::feature_collection;
using FeatureExtensionValue = mapbox::util::variant<Value, FeatureCollection>;

} // namespace mbgl
