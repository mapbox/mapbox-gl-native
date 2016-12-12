#pragma once

#include <mapbox/geometry.hpp>
#include <mapbox/variant.hpp>

namespace mapbox {
namespace geojson {

using value               = mapbox::geometry::value;
using null_value_t        = mapbox::geometry::null_value_t;
using identifier          = mapbox::geometry::identifier;
using point               = mapbox::geometry::point<double>;
using multi_point         = mapbox::geometry::multi_point<double>;
using line_string         = mapbox::geometry::line_string<double>;
using linear_ring         = mapbox::geometry::linear_ring<double>;
using multi_line_string   = mapbox::geometry::multi_line_string<double>;
using polygon             = mapbox::geometry::polygon<double>;
using multi_polygon       = mapbox::geometry::multi_polygon<double>;
using geometry            = mapbox::geometry::geometry<double>;
using geometry_collection = mapbox::geometry::geometry_collection<double>;
using feature             = mapbox::geometry::feature<double>;
using feature_collection  = mapbox::geometry::feature_collection<double>;

// Parse inputs of known types. Instantiations are provided for geometry, feature, and
// feature_collection.
template <class T>
T parse(const std::string &);

// Parse any GeoJSON type.
using geojson = mapbox::util::variant<geometry, feature, feature_collection>;
geojson parse(const std::string &);

// Stringify inputs of known types. Instantiations are provided for geometry, feature, and
// feature_collection.
template <class T>
std::string stringify(const T &);

// Stringify any GeoJSON type.
std::string stringify(const geojson &);

} // namespace geojson
} // namespace mapbox
